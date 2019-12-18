/***************************************************************************
  mapthemesmodel.cpp
  --------------------------------------
  Date                 : Nov 2018
  Copyright            : (C) 2018 by Viktor Sklencar
  Email                : vsklencar at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mapthemesmodel.h"

#include <qgslayertreemodel.h>
#include <qgslayertreenode.h>
#include <qgslayertree.h>
#include <qgsvectorlayer.h>
#include <qgslayertreemodellegendnode.h>
#include <qgsproject.h>
#include "qgsmapthemecollection.h"

#include <QString>

MapThemesModel::MapThemesModel( QgsProject *project, QObject *parent )
  : QAbstractListModel( parent )
  , mProject( project )
{
  reloadMapThemes();
}

MapThemesModel::~MapThemesModel()
{
}

void MapThemesModel::reloadMapThemes()
{
  if ( !mProject ) return;

  QList<QString>allThemes;
  QgsMapThemeCollection *collection = mProject->mapThemeCollection();
  for ( QString name : collection->mapThemes() )
  {
    allThemes << name;
    qDebug() << "Found map theme: " << name;
  }

  if ( mMapThemes != allThemes )
  {
    beginResetModel();
    mMapThemes = allThemes;
    endResetModel();
    emit mapThemesReloaded();
  }
}

void MapThemesModel::updateMapTheme( const QString name )
{
  int row = rowAccordingName( name, 0 );
  if ( row >= 0 )
  {
    setActiveThemeIndex( row );
  }
}

int MapThemesModel::activeThemeIndex() const
{
  return mActiveThemeIndex;
}

void MapThemesModel::setActiveThemeIndex( int activeThemeIndex )
{
  mActiveThemeIndex = activeThemeIndex;
  applyTheme( mMapThemes.at( activeThemeIndex ) );
  emit activeThemeIndexChanged();
}

QVariant MapThemesModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mMapThemes.count() )
    return QVariant();

  switch ( role )
  {
    case Name:
      return mMapThemes.at( index.row() );
  }
  return QVariant();
}

QHash<int, QByteArray> MapThemesModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[Name] = "name";
  return roleNames;
}

QModelIndex MapThemesModel::index( int row, int column, const QModelIndex &parent ) const
{
  Q_UNUSED( column );
  Q_UNUSED( parent );
  return createIndex( row, 0, nullptr );
}

int MapThemesModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent );
  return mMapThemes.count();
}

QList<QString> MapThemesModel::mapThemes() const
{
  return mMapThemes;
}

void MapThemesModel::setMapThemes( const QList<QString> &mapThemes )
{
  if ( mMapThemes == mapThemes )
    return;

  mMapThemes = mapThemes;
  emit mapThemesReloaded();
}

void MapThemesModel::applyTheme( const QString &name )
{
  QgsLayerTree *root = mProject->layerTreeRoot();
  QgsLayerTreeModel model( root );
  mProject->mapThemeCollection()->applyTheme( name, root, &model );
  emit mapThemeChanged( name );
}

int MapThemesModel::rowAccordingName( QString name, int defaultIndex ) const
{
  int index = mMapThemes.indexOf( name );
  if ( index < 0 )
    return defaultIndex;
  else
    return index;
}

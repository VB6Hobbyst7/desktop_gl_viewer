/******************************************************************************
 * Project: libngstore
 * Purpose: NextGIS store and visualization support library
 * Author:  Dmitry Baryshnikov, dmitry.baryshnikov@nextgis.com
 ******************************************************************************
 *   Copyright (c) 2016-2017 NextGIS, <info@nextgis.com>
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include "mapmodel.h"

#include <QDataStream>
#include <QMimeData>

#include "ngstore/codes.h"

constexpr const char* MIME = "application/vnd.map.layer";

MapModel::MapModel(QObject *parent)
    : QAbstractItemModel(parent), m_mapId(-1)
{
}

MapModel::~MapModel()
{
    if(isValid())
        ngsMapClose(m_mapId);
}

bool MapModel::create(const char *name, const char *description,
                      unsigned short epsg, double minX, double minY,
                      double maxX, double maxY)
{
    beginResetModel();
    if(isValid())
        ngsMapClose(m_mapId);
    m_mapId = ngsMapCreate(name, description, epsg, minX, minY, maxX, maxY);
//    const char *options[3] = {"VIEWPORT_REDUCE_FACTOR=1.1",
//                              "ZOOM_INCREMENT=0",
//                              nullptr};
//    char** popt = const_cast<char**>(options);

//    ngsMapSetOptions(m_mapId, popt);

    setSelectionStyle({230, 120, 36, 128}, {230, 120, 36, 255}, 2.5);
    endResetModel();
    return isValid();
}

bool MapModel::open(const char *path)
{
    beginResetModel();
    if(isValid())
        ngsMapClose(m_mapId);
    m_mapId = ngsMapOpen(path);

    const char *options[3] = {"VIEWPORT_REDUCE_FACTOR=1.0",
                              "ZOOM_INCREMENT=-1",
                              nullptr};
    char **popt = const_cast<char**>(options);

    ngsMapSetOptions(m_mapId, popt);

/*  // for test
    setOverlayVisible(MOT_EDIT | MOT_LOCATION, true);

    const char* iconsetName = "iconset";
    if(ngsMapIconSetExists(m_mapId, iconsetName) != 1) {
        std::string iconsetPath =
                std::string(ngsGetCurrentDirectory()) + "/iconset.png";
        if(ngsMapIconSetAdd(m_mapId, iconsetName, iconsetPath.c_str(), 1)
                == COD_SUCCESS) {
            JsonObjectH styleH;
            const char* locationStyleName = "marker";
            int iconWidth = 32;
            int iconHeight = 32;
            double size = 15.0;

            ngsLocationOverlaySetStyleName(m_mapId, locationStyleName);
            styleH = ngsLocationOverlayGetStyle(m_mapId);
            ngsJsonObjectSetStringForKey(styleH, "iconset_name", iconsetName);
            ngsJsonObjectSetIntegerForKey(styleH, "icon_width", iconWidth);
            ngsJsonObjectSetIntegerForKey(styleH, "icon_height", iconHeight);
            ngsJsonObjectSetIntegerForKey(styleH, "icon_index", 5);
            ngsJsonObjectSetDoubleForKey(styleH, "size", size);
            ngsLocationOverlaySetStyle(m_mapId, styleH);

            const char* editPointStyleName = "markerEditPoint";
            enum ngsEditStyleType type = EST_POINT;

            ngsEditOverlaySetStyleName(m_mapId, type, editPointStyleName);
            styleH = ngsEditOverlayGetStyle(m_mapId, type);
            ngsJsonObjectSetStringForKey(styleH, "iconset_name", iconsetName);
            ngsJsonObjectSetIntegerForKey(styleH, "icon_width", iconWidth);
            ngsJsonObjectSetIntegerForKey(styleH, "icon_height", iconHeight);
            ngsJsonObjectSetDoubleForKey(styleH, "size", size);
            ngsJsonObjectSetIntegerForKey(styleH, "point_index", 2);
            ngsJsonObjectSetIntegerForKey(styleH, "selected_point_index", 0);
            ngsJsonObjectSetIntegerForKey(styleH, "walk_point_index", 3);
            ngsJsonObjectSetIntegerForKey(styleH, "median_point_index", 3);
            ngsJsonObjectSetIntegerForKey(
                    styleH, "selected_median_point_index", 1);
            ngsEditOverlaySetStyle(m_mapId, type, styleH);

            const char* editLineStyleName = "editLine";
            type = EST_LINE;
            double line_width = 20.0;

            ngsRGBA lineColor = {255, 128, 255, 255};
            QString lineColorHex;
            lineColorHex.sprintf("#%02x%02x%02x%02x", lineColor.R, lineColor.G,
                    lineColor.B, lineColor.A);

            ngsRGBA selectedLineColor = {64, 192, 255, 255};
            QString selectedLineColorHex;
            selectedLineColorHex.sprintf("#%02x%02x%02x%02x",
                    selectedLineColor.R, selectedLineColor.G,
                    selectedLineColor.B, selectedLineColor.A);

            ngsEditOverlaySetStyleName(m_mapId, type, editLineStyleName);
            styleH = ngsEditOverlayGetStyle(m_mapId, type);
            ngsJsonObjectSetDoubleForKey(styleH, "line_width", line_width);
            ngsJsonObjectSetStringForKey(
                    styleH, "line_color", lineColorHex.toStdString().c_str());
            ngsJsonObjectSetStringForKey(styleH, "selected_line_color",
                    selectedLineColorHex.toStdString().c_str());
            ngsEditOverlaySetStyle(m_mapId, type, styleH);

            const char* editFillStyleName = "editFill";
            type = EST_FILL;

            ngsRGBA fillColor = {128, 92, 50, 255};
            QString fillColorHex;
            fillColorHex.sprintf("#%02x%02x%02x%02x", fillColor.R, fillColor.G,
                    fillColor.B, fillColor.A);

            ngsRGBA selectedFillColor = {100, 215, 40, 255};
            QString selectedFillColorHex;
            selectedFillColorHex.sprintf("#%02x%02x%02x%02x",
                    selectedFillColor.R, selectedFillColor.G,
                    selectedFillColor.B, selectedFillColor.A);

            ngsEditOverlaySetStyleName(m_mapId, type, editFillStyleName);
            styleH = ngsEditOverlayGetStyle(m_mapId, type);
            ngsJsonObjectSetStringForKey(
                    styleH, "fill_color", fillColorHex.toStdString().c_str());
            ngsJsonObjectSetStringForKey(styleH, "selected_fill_color",
                    selectedFillColorHex.toStdString().c_str());
            ngsEditOverlaySetStyle(m_mapId, type, styleH);

            const char* editCrossStyleName = "marker";
            type = EST_CROSS;
            size = 64;

            ngsEditOverlaySetStyleName(m_mapId, type, editCrossStyleName);
            styleH = ngsEditOverlayGetStyle(m_mapId, type);
            ngsJsonObjectSetStringForKey(styleH, "iconset_name", iconsetName);
            ngsJsonObjectSetIntegerForKey(styleH, "icon_width", iconWidth);
            ngsJsonObjectSetIntegerForKey(styleH, "icon_height", iconHeight);
            ngsJsonObjectSetIntegerForKey(styleH, "icon_index", 6);
            ngsJsonObjectSetDoubleForKey(styleH, "size", size);
            ngsEditOverlaySetStyle(m_mapId, type, styleH);

            const char *options[2] = {"CROSS=ON",
                                      nullptr};
            char** popt = const_cast<char**>(options);
            ngsOverlaySetOptions(m_mapId, MOT_EDIT, popt);
        }
    }

    ngsLocationOverlayUpdate(m_mapId, {4185733.6079, 7197616.5748, 0.0}, 0.0, 0.0);
*/
    endResetModel();

    return isValid();
}


QModelIndex MapModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    LayerH childItem = ngsMapLayerGet(m_mapId, row);
    if(nullptr != childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex MapModel::parent(const QModelIndex &/*index*/) const
{
    return QModelIndex(); // Plain list now
}

int MapModel::rowCount(const QModelIndex &/*parent*/) const
{
    return ngsMapLayerCount(m_mapId);
}

int MapModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1; // Only layer name
}

QVariant MapModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    LayerH layer = static_cast<LayerH>(index.internalPointer());
    return ngsLayerGetName(layer);
}

bool MapModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        LayerH layer = static_cast<LayerH>(index.internalPointer());
        if(ngsLayerSetName(layer, value.toString().toUtf8()) != COD_SUCCESS)
            return false;
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags MapModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = Qt::ItemIsEditable | Qt::ItemIsEnabled |
            Qt::ItemIsSelectable;

    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

char MapModel::mapId() const
{
    return m_mapId;
}

void MapModel::setSize(int w, int h, bool YAxisInverted)
{
    if(m_mapId < 0)
        return;
    ngsMapSetSize(m_mapId, w, h, YAxisInverted ? 1 : 0);
    ngsMapSetExtentLimits(m_mapId, -20037508.34, -20037508.34, 20037508.34, 20037508.34);
}

void MapModel::draw(ngsDrawState state, ngsProgressFunc callback,
                       void *callbackData)
{
    if(m_mapId < 0)
        return;
    ngsMapDraw(m_mapId, state, callback, callbackData);
}

void MapModel::invalidate(const ngsExtent& bounds)
{
    if(m_mapId < 0)
        return;
    ngsMapInvalidate(m_mapId, bounds);
}

void MapModel::setBackground(const ngsRGBA &color)
{
    if(m_mapId < 0)
        return;
    ngsMapSetBackgroundColor(m_mapId, color);
}

ngsCoordinate MapModel::getCenter() const
{
    if(m_mapId < 0)
        return {0, 0, 0};
    return ngsMapGetCenter(m_mapId);
}

bool MapModel::setCenter(const ngsCoordinate &newCenter)
{
    if(m_mapId < 0)
        return false;
    return ngsMapSetCenter(m_mapId, newCenter.X, newCenter.Y);
}

ngsCoordinate MapModel::getCoordinate(int x, int y) const
{
    if(m_mapId < 0)
        return {0, 0, 0};
    return ngsMapGetCoordinate(m_mapId, static_cast<double>(x),
                               static_cast<double>(y));
}

ngsCoordinate MapModel::getDistance(const QPoint &pt) const
{
    if(m_mapId < 0)
        return {0, 0, 0};
    return ngsMapGetDistance(m_mapId, pt.x(), pt.y());
}

double MapModel::getRotate(ngsDirection dir) const
{
    if(m_mapId < 0)
        return 0.0;
    return ngsMapGetRotate(m_mapId, dir);
}

bool MapModel::setRotate(ngsDirection dir, double value)
{
    if(m_mapId < 0)
        return false;
    return ngsMapSetRotate(m_mapId, dir, value) == COD_SUCCESS;
}

double MapModel::getScale() const
{
    if(m_mapId < 0)
        return 1.0;
    return ngsMapGetScale(m_mapId);
}

bool MapModel::setScale(double value)
{
    if(m_mapId < 0)
        return false;
    return ngsMapSetScale(m_mapId, value);
}

void MapModel::createLayer(const char *name, const char *path)
{
    if(m_mapId < 0)
        return;
    int result = ngsMapCreateLayer(m_mapId, name, path);
    if(-1 != result) {
        beginInsertRows(QModelIndex(), result, result);
        insertRow(result);
        endInsertRows();
    }
}

void MapModel::deleteLayer(const QModelIndex &index)
{
    if(m_mapId < 0)
        return;
    LayerH layer = static_cast<LayerH>(index.internalPointer());
    beginRemoveRows(index.parent(), index.row(), index.row());
    if(ngsMapLayerDelete(m_mapId, layer) == COD_SUCCESS) {
        removeRow(index.row());
    }
    endRemoveRows();
}

void MapModel::undoEdit()
{
    if(m_mapId < 0)
        return;
    if (ngsEditOverlayUndo(m_mapId)) {
        emit undoEditFinished();
    }
}

void MapModel::redoEdit()
{
    if(m_mapId < 0)
        return;
    if (ngsEditOverlayRedo(m_mapId)) {
        emit redoEditFinished();
    }
}

bool MapModel::canUndoEdit()
{
    if(m_mapId < 0)
        return false;
    return ngsEditOverlayCanUndo(m_mapId);
}

bool MapModel::canRedoEdit()
{
    if(m_mapId < 0)
        return false;
    return ngsEditOverlayCanRedo(m_mapId);
}

void MapModel::saveEdit()
{
    if(m_mapId < 0)
        return;
    ngsEditOverlaySave(m_mapId);
    emit editSaved();
}

void MapModel::cancelEdit()
{
    if(m_mapId < 0)
        return;
    if (ngsEditOverlayCancel(m_mapId)) {
        emit editCanceled();
    }
}

void MapModel::createNewGeometry(const QModelIndex& index, bool walkMode)
{
    if(m_mapId < 0)
        return;
    LayerH layer = static_cast<LayerH>(index.internalPointer());
    if(ngsEditOverlayCreateGeometryInLayer(m_mapId, layer, walkMode) ==
            COD_SUCCESS) {
        emit geometryCreated(index, walkMode);
    }
}

void MapModel::editSelectedGeometry()
{
    if(m_mapId < 0)
        return;
    if (ngsEditOverlayEditGeometry(m_mapId, nullptr, -1) == COD_SUCCESS) {
        emit geometryEditStarted();
    }
}

void MapModel::deleteGeometry()
{
    if(m_mapId < 0)
        return;
    ngsEditOverlayDeleteGeometry(m_mapId);
    emit geometryDeleted();
}

void MapModel::addPoint()
{
    if(m_mapId < 0)
        return;
    if (ngsEditOverlayAddPoint(m_mapId) == COD_SUCCESS) {
        emit pointAdded();
    }
}

void MapModel::addVertex(const ngsCoordinate& coordinates)
{
    if(m_mapId < 0)
        return;
    if (ngsEditOverlayAddVertex(m_mapId, coordinates) == COD_SUCCESS) {
        emit pointAdded();
    }
}

void MapModel::deletePoint()
{
    if(m_mapId < 0)
        return;
    ngsEditOverlayDeletePoint(m_mapId);
    emit pointDeleted();
}

void MapModel::addHole()
{
    if(m_mapId < 0)
        return;
    if (ngsEditOverlayAddHole(m_mapId) == COD_SUCCESS) {
        emit holeAdded();
    }
}

void MapModel::deleteHole()
{
    if(m_mapId < 0)
        return;
    ngsEditOverlayDeleteHole(m_mapId);
    emit holeDeleted();
}

void MapModel::addGeometryPart()
{
    if(m_mapId < 0)
        return;
    if (ngsEditOverlayAddGeometryPart(m_mapId) == COD_SUCCESS) {
        emit geometryPartAdded();
    }
}

void MapModel::deleteGeometryPart()
{
    if(m_mapId < 0)
        return;
    ngsEditOverlayDeleteGeometryPart(m_mapId);
    emit geometryPartDeleted();
}

ngsPointId MapModel::editOverlayTouch(double x, double y, const ngsMapTouchType type)
{
    if(m_mapId < 0)
        return {-1, 0};
    return ngsEditOverlayTouch(m_mapId, x, y, type);
}

void MapModel::setSelectionStyle(const ngsRGBA& fillColor,
                                 const ngsRGBA& borderColor, double width)
{
    if(m_mapId < 0)
        return;
    QString sColor;
    sColor.sprintf("#%02x%02x%02x%02x", borderColor.R, borderColor.G,
                   borderColor.B, borderColor.A);

    JsonObjectH style = ngsMapGetSelectionStyle(m_mapId, ST_POINT);
    ngsJsonObjectSetIntegerForKey(style, "type", 3);
    ngsJsonObjectSetStringForKey(style, "color", sColor.toStdString().c_str());
    ngsJsonObjectSetDoubleForKey(style, "size", width * 2.0);
    ngsMapSetSelectionsStyle(m_mapId, ST_POINT, style);
    ngsJsonObjectFree(style);

    style = ngsMapGetSelectionStyle(m_mapId, ST_LINE);
    ngsJsonObjectSetStringForKey(style, "color", sColor.toStdString().c_str());
    ngsJsonObjectSetDoubleForKey(style, "line_width", width);
    ngsMapSetSelectionsStyle(m_mapId, ST_LINE, style);
     ngsJsonObjectFree(style);

    style = ngsMapGetSelectionStyle(m_mapId, ST_FILL);
    JsonObjectH lineStyle = ngsJsonObjectGetObject(style, "line");
    ngsJsonObjectSetStringForKey(lineStyle, "color", sColor.toStdString().c_str());
    ngsJsonObjectSetDoubleForKey(lineStyle, "line_width", width);

    sColor.sprintf("#%02x%02x%02x%02x", fillColor.R, fillColor.G,
                   fillColor.B, fillColor.A);
    JsonObjectH fillStyle = ngsJsonObjectGetObject(style, "fill");
    ngsJsonObjectSetStringForKey(fillStyle, "color", sColor.toStdString().c_str());

    ngsMapSetSelectionsStyle(m_mapId, ST_FILL, style);
    ngsJsonObjectFree(style);
}

QVector<Layer> MapModel::identify(double minX, double minY,
                                                 double maxX, double maxY)
{
    QVector<Layer> out;
    if(m_mapId < 0)
        return out;
    int count = ngsMapLayerCount(m_mapId);
    for(int i = 0; i < count; ++i) {
        LayerH layerH = ngsMapLayerGet(m_mapId, i);
        CatalogObjectH ds = ngsLayerGetDataSource(layerH);
        enum ngsCatalogObjectType type = ngsCatalogObjectType(ds);
        if(isFeatureClass(type)) {
            ngsFeatureClassSetSpatialFilter(ds, minX, minY, maxX, maxY);
            FeatureH f;
            Layer layer(layerH);
            while((f = ngsFeatureClassNextFeature(ds)) != nullptr) {
                layer.addFeatureToSet(FeaturePtr(new Feature(f)));
            }
            ngsFeatureClassSetFilter(ds, nullptr, nullptr);
            if(!layer.featureSet().empty()) {
                out.append(layer);
            }
        }
    }

    return out;
}

bool MapModel::isFeatureClass(enum ngsCatalogObjectType type) const
{
    return type >= CAT_FC_ANY && type <= CAT_FC_ALL;
}

QStringList MapModel::mimeTypes() const
{
    QStringList types;
    types << MIME;
    return types;
}

bool MapModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                            int row, int /*column*/, const QModelIndex &parent)
{
    if(m_mapId < 0)
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat(MIME))
        return false;

    LayerH beforeLayer = parent.internalPointer();

    QByteArray encodedData = data->data(MIME);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

     while (!stream.atEnd()) {
         qint64 pointer;
         stream >> pointer;
         LayerH movedLayer = reinterpret_cast<LayerH>(pointer);

         bool result;
         beginResetModel();
         result = ngsMapLayerReorder(m_mapId, beforeLayer, movedLayer) ==
                              COD_SUCCESS;
         endResetModel();
         return result;
     }

    return false;
}


QMimeData *MapModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

     foreach (QModelIndex index, indexes) {
         if (index.isValid()) {
             qint64 pointer = reinterpret_cast<qint64>(index.internalPointer());
             stream << pointer;
         }
     }

     mimeData->setData(MIME, encodedData);
     return mimeData;
}

void MapModel::setOverlayVisible(int typeMask, char visible)
{
    if(m_mapId < 0)
        return;
    ngsOverlaySetVisible(m_mapId, typeMask, visible);
}

//------------------------------------------------------------------------------
// Layer
//------------------------------------------------------------------------------

void Layer::setSelection(const QSet<long long> &ids)
{
    if(ids.empty()) {
        ngsLayerSetSelectionIds(m_handle, nullptr, 0);
        return;
    }

    long long idsp[ids.size()];
    int counter = 0;
    for(auto id : ids) {
        idsp[counter++] = id;
    }
    ngsLayerSetSelectionIds(m_handle, idsp, ids.size());
}

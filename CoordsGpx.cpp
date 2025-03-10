/*  This file is part of TSRE5.
 *
 *  TSRE5 - train sim game engine and MSTS/OR Editors. 
 *  Copyright (C) 2016 Piotr Gadecki <pgadecki@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *
 *  See LICENSE.md or https://www.gnu.org/licenses/gpl.html
 */

#include "CoordsGpx.h"
#include <QFile>
#include "GLMatrix.h"
#include <math.h>
#include "ParserX.h"
#include <QDebug>
#include <cstdlib>
#include "Vector2f.h"
#include "TerrainLib.h"
#include "TS.h"
#include "Game.h"
#include "FileFunctions.h"
#include "ReadFile.h"
#include "GeoCoordinates.h"

CoordsGpx::CoordsGpx(QString path) {
    qDebug() << "gpx!";
    loaded = false;

    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "no file" << file.errorString();
        return;
    }
    qDebug() <<  "file";
    QByteArray data = file.readAll();
    
    bool placemark = false;
    bool line = false;
    bool coordinates = false;
    bool nametext = false;
    int iii = 0, uuu = 0;
    IghCoordinate* igh;
    PreciseTileCoordinate* ppp;
    
    QXmlStreamReader reader((data));
    reader.readNext();
    QString name;
    QXmlStreamAttributes attr;
    while (!reader.isEndDocument()) {
        if (reader.isStartElement()) {
            name = reader.name().toString();
            attr = reader.attributes();
            
            if (name.toUpper() == ("WPT")) {
                placemark = true;
                markerList.push_back(Marker());
                float lat = 0;
                float lon = 0;
                if(attr.size() == 2){
                    if(attr[0].name().toString().toUpper() == "LAT")
                        lat = attr[0].value().toFloat();
                    if(attr[0].name().toString().toUpper() == "LON")
                        lon = attr[0].value().toFloat();
                    if(attr[1].name().toString().toUpper() == "LAT")
                        lat = attr[1].value().toFloat();
                    if(attr[1].name().toString().toUpper() == "LON")
                        lon = attr[1].value().toFloat();
                    igh = Game::GeoCoordConverter->ConvertToInternal(lat, lon);
                    ppp = Game::GeoCoordConverter->ConvertToTile(igh);
                    markerList.back().lat = lat;
                    markerList.back().lon = lon;
                    markerList.back().type = 0;
                    markerList.back().tileX.push_back(ppp->TileX);
                    markerList.back().tileZ.push_back(ppp->TileZ);
                    markerList.back().x.push_back(ppp->X*2048-1024);
                    markerList.back().y.push_back(0);
                    markerList.back().z.push_back(ppp->Z*2048-1024);
                }
            } else if (name.toUpper() == ("TRK")) {
                placemark = true;
                markerList.push_back(Marker());
            } else if (name.toUpper() == ("TRKPT")) {
                float lat = 0;
                float lon = 0;
                if(attr.size() == 2){
                    if(attr[0].name().toString().toUpper() == "LAT")
                        lat = attr[0].value().toFloat();
                    if(attr[0].name().toString().toUpper() == "LON")
                        lon = attr[0].value().toFloat();
                    if(attr[1].name().toString().toUpper() == "LAT")
                        lat = attr[1].value().toFloat();
                    if(attr[1].name().toString().toUpper() == "LON")
                        lon = attr[1].value().toFloat();
                    igh = Game::GeoCoordConverter->ConvertToInternal(lat, lon);
                    ppp = Game::GeoCoordConverter->ConvertToTile(igh);
                    if(markerList.back().tileX.size() == 0){
                        markerList.back().lat = lat;
                        markerList.back().lon = lon;
                        markerList.back().type = 0;
                    }
                    markerList.back().tileX.push_back(ppp->TileX);
                    markerList.back().tileZ.push_back(ppp->TileZ);
                    markerList.back().x.push_back(ppp->X*2048-1024);
                    markerList.back().y.push_back(0);
                    markerList.back().z.push_back(ppp->Z*2048-1024);
                }
            } else if (name.toUpper() == ("NAME")) {
                if(placemark){
                    nametext = true;
                }
            }
        } else if (reader.isEndElement()) {
            name = reader.name().toString();
            if (name.toUpper() == ("WPT")) {
                placemark = false;
            }
            if (name.toUpper() == ("TRK")) {
                placemark = false;
            } 
            if (name.toUpper() == ("NAME")) {
                if(placemark){
                    nametext = false;
                }
            }
        } else if (reader.isCharacters()) {
            if(nametext){
                //qDebug() << reader.text();
                markerList.back().name = reader.text().toString();
            }
        }
        
        reader.readNext();
    }
    loaded = true;
}

CoordsGpx::~CoordsGpx() {
}


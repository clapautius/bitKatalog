/***************************************************************************
 *   Copyright (C) 2004 by Tudor Marian Pristavu                           *
 *   tudor.mp@home.ro                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#if !defined(_plugins_h_)
#define _plugins_h_

#include <string>

using namespace std;

int sha1Callback(string fileName, string *pParam, string *pValue,
                 volatile const bool *pAbortFlag = NULL);

int sha256Callback(string fileName, string *pParam, string *pValue,
                   volatile const bool *pAbortFlag = NULL);

int sha1UsingBufCallback(const char *buf, unsigned bufLen, bool isFirstChunk,
                         bool isLastChunk, string *, string *,
                         volatile const bool *pAbortFlag = NULL);

int sha256UsingBufCallback(const char *buf, unsigned bufLen, bool isFirstChunk,
                           bool isLastChunk, string *, string *,
                           volatile const bool *pAbortFlag = NULL);

#endif

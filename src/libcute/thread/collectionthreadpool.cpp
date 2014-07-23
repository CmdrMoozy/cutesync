/*
 * CuteSync - A media library management and synchronization application.
 * Copyright (C) 2011 Axel Rasmussen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "collectionthreadpool.h"

#include <QThread>

#include "libcute/collections/collectiontyperesolver.h"
#include "libcute/widgets/collectionlistitem.h"

CSCollectionThreadPool::CSCollectionThreadPool(QObject *p)
	: QObject(p)
{
	thread = new QThread(this);
	thread->start();

	resolver = new CSCollectionTypeResolver();
	resolver->moveToThread(thread);
}

CSCollectionThreadPool::~CSCollectionThreadPool()
{
	delete resolver;
}

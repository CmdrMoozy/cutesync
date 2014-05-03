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

#include <iostream>

extern "C"
{
	#include <glib.h>
	#include <gpod-1.0/gpod/itdb.h>
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		std::cout << "You must give a directory to initialize an iPod in.\n";
		return 1;
	}

	GError *error = NULL;
	if( !itdb_init_ipod( argv[1], "C297", "mkipod", &error) )
	{
		if(error != NULL)
		{
			std::cout << "Error while initializing iPod: " << error->message << "\n";

			g_error_free(error);
			error = NULL;
			return 1;
		}
		else
		{
			std::cout << "An unknown error was encountered while initializing iPod!\n";
			return 1;
		}
	}

	std::cout << "iPod created successfully.\n";

	return 0;
}

/*****
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****/

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <SDL/SDL_types.h>
#include <SDL/SDL_net.h>
#include "btree.h"
#include <string>


// For easy packet parsing
#define GET_UINT32(x) (SDL_SwapBE32(*(Uint32*)(x)))
#define GET_UINT16(x) (SDL_SwapBE16(*(Uint16*)(x)))
#define GET_UINT8(x) (*(Uint8*)(x))
#define GET_SINT8(x) (*(Sint8*)(x))

class Network
{
public:
  Network ();
  ~Network ();
  void connect (std::string & host, Uint16 port);
  void connect (Uint32 ip, Uint16 port);
  void disconnect ();
  int ready ();
  int read (Uint8 * pvOut, int len);
  void write (Uint8 * pvIn, int len);

  TCPsocket uo;			/**< Our socket to the server */
  bool readCompressed;		/**< whether or not we're decompressing */

private:
  bTree tree,			/**< The big bTree for decompression */
   *readCur;			/**< Current location in the bTree */
  SDLNet_SocketSet sockets;	/**< Socket Set for testing activity */
  int readBits;			/**< Current bit read */
  Uint16 readValue;		/**< Current value read */
};

#endif

/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// net_dgrm.h


EXTERN_CPP int			Datagram_Init (void);
EXTERN_CPP void		Datagram_Listen (qboolean state);
EXTERN_CPP void		Datagram_SearchForHosts (qboolean xmit);
EXTERN_CPP qsocket_t	*Datagram_Connect (char *host);
EXTERN_CPP qsocket_t 	*Datagram_CheckNewConnections (void);
EXTERN_CPP int			Datagram_GetMessage (qsocket_t *sock);
EXTERN_CPP int			Datagram_SendMessage (qsocket_t *sock, sizebuf_t *data);
EXTERN_CPP int			Datagram_SendUnreliableMessage (qsocket_t *sock, sizebuf_t *data);
EXTERN_CPP qboolean	Datagram_CanSendMessage (qsocket_t *sock);
EXTERN_CPP qboolean	Datagram_CanSendUnreliableMessage (qsocket_t *sock);
EXTERN_CPP void		Datagram_Close (qsocket_t *sock);
EXTERN_CPP void		Datagram_Shutdown (void);
# Copyright 1999-2004 Gentoo Foundation 
# Distributed under the terms of the GNU General Public License v2 
# $Header: $ 

inherit eutils games 

DESCRIPTION="Ultima Iris is an open source, fully 3D client for the MMORPG Ultim 
a Online." 
HOMEPAGE="http://ultimairis.sourceforge.net/" 

SRC_URI="http://cvs.sourceforge.net/cvstarballs/ultimairis-cvsroot.tar.bz2" 

LICENSE="GPL-2" 
SLOT="0" 
KEYWORDS="x86" 
IUSE="" 
DEPEND=">=media-libs/libsdl-1.2.7-r3 
   >=media-libs/sdl-image-1.2.3-r1 
   >=media-libs/sdl-mixer-1.2.5-r1 
   >=media-libs/sdl-net-1.2.5 
   >=media-libs/sdl-ttf-2.0.6" 

#RDEPEND="" 
S="${WORKDIR}/ultimairis/ui5" 

pkg_setup() { 
   games_pkg_setup 
    
} 

src_unpack() { 
   unpack ultimairis-cvs-snapshot.tar.gz 

} 

src_compile() { 
   # This kills the nasty .v extension CVS puts on everything. 
   # It should probably go somewhere else, but I don't care enough to find out where. 
    
   einfe "The following command is probably going to cause some errors. Ignore them" 
   find . -name "*,v" -exec sh -c 'mv $0 `echo $0 | sed s/,v$//`' {} \; 
   einfo "There, the command is done. Any more errors are now your problem." 
    
   econf || die "econf failed" 
   emake || die "emake failed" 
    
} 

src_install() { 
   make -C src \ 
      DEBUGCFLAGS= \ 
      CFLAGS="${CFLAGS}" \ 
      bindir="${D}${GAMES_BINDIR}" \ 
      datadir="${D}/usr/share" \ 
      libdir="${D}${GAMES_LIBDIR}" \ 
      install || die "make install failed" 
       
   einfo "Please note, to run Iris you need the data files" 
   einfo "from the official client, Ultima Online: Third Dawn." 
    
   einfo "Remember to edit config.xml to tell Iris where to" 
   einfo "find the data files." 

   prepgamesdirs 
    
}
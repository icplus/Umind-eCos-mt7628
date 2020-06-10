//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2004 Free Software Foundation, Inc.                        
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================

/* Simple test-case for the BSD socket API  : echo on TCP port 7 */

#include <lwip/sys.h>
#undef LWIP_COMPAT_SOCKETS
#define LWIP_COMPAT_SOCKETS 1
#include <lwip/sockets.h>
#include <lwip/inet.h>
#include <cyg/infra/testcase.h>

#ifdef CYGPKG_LWIP_TCP

char buf[400];
static void
socket_thread(void *arg)
{
	int sock,s;
	int len;
	struct sockaddr_in addr,rem;
	sock = socket(AF_INET,SOCK_STREAM,0);	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7);
	addr.sin_addr.s_addr = INADDR_ANY;

	bind(sock,(struct sockaddr *)&addr,sizeof(addr));
			
	listen(sock,5);
	while(1) {	
		len = sizeof(rem);
		s = accept(sock,(struct sockaddr*)&rem,&len);
		while((len = read(s,buf,400)) > 0)
			write(s,buf,len);
		close(s);
	}	
}

void
tmain(cyg_addrword_t p)
{
  lwip_init();	
  sys_thread_new(socket_thread, (void*)"socket",7);
}

#define STACK_SIZE 0x1000
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

void
socket_main(void)
{
    CYG_TEST_INIT();
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(10,                // Priority - just a number
                      tmain,          // entry
                      0,                 // entry parameter
                      "socket echo test",        // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    cyg_scheduler_start();
    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{
    socket_main();
}

#else // def CYGPKG_LWIP_TCP
#define N_A_MSG "TCP support disabled"
#endif // def CYGFUN_KERNEL_API_C

#ifdef N_A_MSG
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA(N_A_MSG);
}
#endif // N_A_MSG


/* ********************************************************************** */
/*                                                                        */
/*   ToolBar  Main Module                                                 */
/*                                                                        */
/*   This sample program implements two menu bars: a standard action bar  */
/*   and a "toolbar". The toolbar consists of "buttons" that can be       */
/*   pressed. Currently the buttons take no actions.                      */
/*                                                                        */
/*   The purpose of this sample is to demonstrate subclassing the frame   */
/*   window to add frame controls.                                        */
/*                                                                        */
/*                                                                        */
/*   DISCLAIMER OF WARRANTIES.  The following [enclosed] code is          */
/*   sample code created by IBM Corporation. This sample code is not      */
/*   part of any standard or IBM product and is provided to you solely    */
/*   for  the purpose of assisting you in the development of your         */
/*   applications.  The code is provided "AS IS", without                 */
/*   warranty of any kind.  IBM shall not be liable for any damages       */
/*   arising out of your use of the sample code, even if they have been   */
/*   advised of the possibility of   such damages.                        */
/*                                                                        */
/*   Copyright 1992, IBM Corp                                             */
/* ********************************************************************** */


#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#define INCL_DOSMISC
#define INCL_DOSERRORS
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include "toolbar.h"




HAB     hab;
HMQ     hmq;
HWND    hwndClient;
HWND    hwndFrame;
HWND    hwndToolBar ;
HWND    hwndMenuBar ;
QMSG    qmsg;

PSZ    szClassName  = "ToolBarClass" ;
PSZ    szMainTitle  = "ToolBar" ;
PSZ    szErrorTitle = "ToolBar Error" ;

        /* ----------------  Prototypes  ------------------------ */
MRESULT EXPENTRY MainWindowProc( HWND, USHORT, MPARAM, MPARAM );
MRESULT EXPENTRY NewFrameProc( HWND, USHORT, MPARAM, MPARAM );
VOID             ShowErrorWindow( PSZ, BOOL );
 


/* ********************************************************************** */
/*                                                                        */
/*   Main                                                                 */
/*                                                                        */
/* ********************************************************************** */

VOID main()
{

  if ( (hab = WinInitialize( 0L )) == (HAB) NULL ){
     printf( "ToolBar Error:  WinInitialize failed \n" );
     return;
  }
  else {
     if ( (hmq = WinCreateMsgQueue( hab, 0 )) == (HMQ) NULL ){
        printf( "ToolBar Error:  WinCreateMsgQueue failed \n" );
        return;
     }
     else {

       ULONG fulCreate= FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
                        FCF_MINMAX | FCF_SHELLPOSITION | FCF_ICON  ;
               /*
                *  Note: no menu was specificed in create flags
                */

        WinSetPointer( HWND_DESKTOP,
                       WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,TRUE));

        WinRegisterClass(hab, szClassName, (PFNWP)MainWindowProc, CS_SIZEREDRAW, 0);

        hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                       0L,
                                       (PULONG)&fulCreate,
                                       szClassName ,
                                       szMainTitle,
                                       0L,
                                       (HMODULE)NULL,
                                       ID_MAIN_WIN,
                                       &hwndClient);
        if ( hwndFrame == NULLHANDLE ) {
           ShowErrorWindow( "Error creating Main window !", TRUE );
        }
        else {
           PFNWP     pfnwpOldFrameProc ;

             /* ---------  subclass frame proc  ------------------ */
           pfnwpOldFrameProc = WinSubclassWindow( hwndFrame,
                                                  (PFNWP) NewFrameProc );
           if ( pfnwpOldFrameProc == (PFNWP)0L ){
               ShowErrorWindow( "Error subclassing frame window !", TRUE );
           }
           else {
              PID       pid ;
              SWCNTRL   swCntrl;
              HSWITCH   hSwitch ;
              LONG      lRGB;

                /* -------  store old frame proc with handle  ------- */
              WinSetWindowULong( hwndFrame,
                                 QWL_USER,
                                 (ULONG) pfnwpOldFrameProc );

                /* ------------------ load menus  ------------------- */
              hwndMenuBar = WinLoadMenu( hwndFrame,
                                         (HMODULE)NULL,
                                         MID_MENUBAR );

              hwndToolBar = WinLoadMenu( hwndFrame,
                                         (HMODULE)NULL,
                                         MID_TOOLBAR );
                /*
                 *  Note that the last menu loaded, the toolbar, is the
                 *  one that is associated with the frame as "the" menu.
                 *  this means that hwndMenuBar is the only link to the
                 *  regular action bar, so hang onto it tightly
                 */

                /* ---------- set toolbar background color ---------- */
              lRGB =  WinQuerySysColor( HWND_DESKTOP, SYSCLR_BUTTONDARK, 0L );
              WinSetPresParam( hwndToolBar,
                               PP_BACKGROUNDCOLOR,
                               4L,
                               (PVOID)lRGB );

                /* ---------  set window size and pos  -------------- */
              WinSetWindowPos( hwndFrame,
                               HWND_TOP,
                               0, 0, 370, 300, 
                               SWP_SIZE | SWP_SHOW | SWP_ACTIVATE );

               /* ----------- add program to tasklist  --------------- */
              WinQueryWindowProcess( hwndFrame, &pid, NULL );
              swCntrl.hwnd = hwndFrame ;
              swCntrl.hwndIcon = (HWND) NULL ;
              swCntrl.hprog = (HPROGRAM) NULL ;
              swCntrl.idProcess = pid ;
              swCntrl.idSession = (LONG) NULL ;
              swCntrl.uchVisibility = SWL_VISIBLE ;
              swCntrl.fbJump = SWL_JUMPABLE ;
              sprintf( swCntrl.szSwtitle, szMainTitle );
              hSwitch = WinAddSwitchEntry((PSWCNTRL)&swCntrl);


              WinSetPointer(HWND_DESKTOP,
                            WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,TRUE));

                 /* ---------- start the main processing loop ----------- */
              while (WinGetMsg(hab, &qmsg,NULLHANDLE,0,0)){
                  WinDispatchMsg(hab, &qmsg);
              }

              WinRemoveSwitchEntry( hSwitch );
           } /* end of else ( pfnwpOldFrameProc ) */

           WinSetPointer(HWND_DESKTOP,
                         WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,TRUE));
           WinDestroyWindow(hwndFrame);
        }  /* end of else (hwndFrame == NULLHANDLE) */

        WinSetPointer(HWND_DESKTOP,
                      WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,TRUE));
        WinDestroyMsgQueue(hmq);
     }  /* end of else ( ...WinCreateMsgQueue() */

   WinTerminate(hab);
   }  /* end of else (...WinInitialize(NULL) */
}  /*  end of main() */

/* ********************************************************************** */
/*                                                                        */
/*   MainWindowProc                                                       */
/*                                                                        */
/* ********************************************************************** */

MRESULT EXPENTRY
MainWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{

  switch (msg) {

    case WM_PAINT:
       {
         RECTL rectl ;
         HPS   hps;

         hps = WinBeginPaint( hwnd, (HPS) NULL, &rectl );
         WinFillRect( hps, (PRECTL)&rectl, SYSCLR_WINDOW);
         WinEndPaint( hps );
      }
      break;

    default:
      return WinDefWindowProc(hwnd,msg,mp1,mp2);

  } /*  end of switch () */
  return( FALSE );

} /*  end of MainWindowProc */
/* ********************************************************************** */
/*                                                                        */
/*   NewFrameProc                                                         */
/*                                                                        */
/*       This frame proc subclasses the original frame proc. The two      */
/*   messages of interest are WM_QUERYFRAMECTLCOUNT and WM_FORMATFRAME.   */
/*   By catching WM_QUERYFRAMECTLCOUNT, we can return the count of the    */
/*   total number of frame controls, both original and new. This count    */
/*   is used to allocate SWP structures for the array during frame        */
/*   formating. When we process WM_FORMATFRAME, we first call the         */
/*   the original frame proc to setup the SWP array for the original      */
/*   frame controls. Note that among these "original" frame controls is   */
/*   the toolbar. We will add the regular action bar as a second menu     */
/*   below the toolbar. The original processing of WM_FORMATFRAME will    */
/*   return the count of original frame controls (note that because we    */
/*   modified WM_QUERYFRAMECTLCOUNT the SWP array will actually contain   */
/*   an extra SWP for our second menu). There should probably be more     */
/*   code that checks the order of controls in the array, but we are      */
/*   going to make some (educated) assumptions about the control order.   */
/*   The client window is always the last item in the SWP array. The      */
/*   menu bar (toolbar in our case) is the previous item. We will use     */
/*   the default settings of these controls to position our new menu and  */
/*   make adjustments to these controls for the new menu.                 */
/*                                                                        */
/* ********************************************************************** */

MRESULT EXPENTRY
NewFrameProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
  PFNWP   oldFrameProc ;

     /* ------- get original frame procedure  --------- */
  oldFrameProc = (PFNWP) WinQueryWindowULong( hwnd, QWL_USER );

  switch (msg) {
    case WM_QUERYFRAMECTLCOUNT :
         {
           USHORT   itemCount ;
           
                /* ---- get count of original frame controls --- */
           itemCount = SHORT1FROMMR( oldFrameProc( hwnd, msg, mp1, mp2 ));
                /* ------- add 1 for new toolbar control  ------ */
           return ( (MRESULT) ++itemCount );  
         }

    case WM_FORMATFRAME :
       {
         PSWP     pSWP ;
         PRECTL   pFrameRect ;
         USHORT   itemCount ;

         pFrameRect = PVOIDFROMMP( mp2 );
         pSWP = PVOIDFROMMP( mp1 );
           /* ---- run regular processing on original controls --- */
         itemCount = SHORT1FROMMR( oldFrameProc( hwnd, msg, mp1, mp2 ));

         /* ---- get size values for 2nd menu bar  -------- */
         pSWP[ itemCount ].fl = SWP_SIZE;
         pSWP[ itemCount ].cy =  pSWP[(itemCount-1)].cy ;  /* set some */
         pSWP[ itemCount ].cx =  pSWP[(itemCount-1)].cx ;  /*  defaults */
         pSWP[ itemCount ].hwndInsertBehind = HWND_TOP ;
         pSWP[ itemCount ].hwnd = hwndMenuBar ;
         WinSendMsg( hwndMenuBar,                 /* let the menu code make */
                     WM_ADJUSTWINDOWPOS,          /*  the actual size       */
                     MPFROMP( pSWP+itemCount ),   /*  adjustments           */
                     (MPARAM) 0L );

          /* ------ position menu directly below other menu  ------- */
         pSWP[itemCount].x = pSWP[(itemCount-2)].x ; 
         pSWP[itemCount].y = pSWP[(itemCount-2)].y - pSWP[itemCount].cy ;
         pSWP[itemCount].fl = pSWP[(itemCount-2)].fl ;
          /* --------  adjust client window size for 2nd menu ------- */
         pSWP[(itemCount-1)].cy -= pSWP[itemCount].cy ;
          /* --------  return total count of frame controls   ------- */
         return( MRFROMSHORT( ++itemCount )  );
       }
       break;

    default:
      return oldFrameProc(hwnd,msg,mp1,mp2);

  } /* end of switch () */

  return( FALSE );

} /* end of NewFrameProc */
/* ********************************************************************** */
/*                                                                        */
/*   ShowErrorWindow                                                      */
/*                                                                        */
/* ********************************************************************** */
VOID
ShowErrorWindow( PSZ  pszErrorMsg, BOOL bUseLastError )
{
  CHAR      acErrorBuffer[256] ;

  if ( bUseLastError ) {
      ERRORID   errorID = WinGetLastError( hab );

      sprintf( acErrorBuffer,
               "%s \n(code = 0x%lX)",
               pszErrorMsg,
               (ULONG) errorID );
      pszErrorMsg = (PSZ) acErrorBuffer ;
  }  /* end of if ( bUseLastError ) */

  WinMessageBox( HWND_DESKTOP,
                 HWND_DESKTOP,
                 pszErrorMsg ,
                 szErrorTitle ,
                 0,
                 MB_CUACRITICAL | MB_OK );

}



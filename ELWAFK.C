/*****************************************************************************
 *                                                                           *
 *       ELWAFK.C - Allows users to notify others when they are away from    *
 *                  the keyboard and when they return using global commands. *
 *                                                                           *
 * Developed 08/04/95 by 'Easy Ed' Erdman.   Major BBS developer code 'EZE'. *
 *                                                                           *
 *       Updates : 10/21/95 - Version 1.1                                    *
 *                 Add option to set "Busy" flag while AFK and restore when  *
 *                       BAK.                                                *
 *                 5/5/96   - Version 1.2                                    *
 *                 Bug fix in setting AFK location for Worldgroup            *
 *                 5/27/96  - Version 1.3                                    *
 *                 Change DEMO period from 2 hours to 7 days                 *
 *                 Change method of generating access code                   *
 *                 2/12/97  - Version 1.3W                                   *
 *                 Add compatibility with Worldgroup 3.0 WIN NT/95           *
 *                 5/14/2021 - Version 1.4                                   *
 *                 Worldgroup 3.2 version, bug fixes, allow both afk loc     *
 *                 and teleconference actions                                *
 *                 6/13/2024 - Version 1.5                                   *
 *                 WG3.2 version and new MBBS V10 version open sourced       *
 *                                                                           *
 *       Copyright (C) 1995 by EzSoft. All rights reserved.                  *
 *       Copyright (C) 2004-2024 Elwynor Technologies. All Rights Reserved.  *
 *                                                                           *
 * This program is free software: you can redistribute it and/or modify      * 
 * it under the terms of the GNU Affero General Public License as published  *
 * by the Free Software Foundation, either version 3 of the License, or      *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU Affero General Public License for more details.                       *
 *                                                                           *
 * You should have received a copy of the GNU Affero General Public License  *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.     *
 *                                                                           *
 * Additional Terms for Contributors:                                        *
 * 1. By contributing to this project, you agree to assign all right, title, *
 *    and interest, including all copyrights, in and to your contributions   *
 *    to Rick Hadsall and Elwynor Technologies.                              *
 * 2. You grant Rick Hadsall and Elwynor Technologies a non-exclusive,       *
 *    royalty-free, worldwide license to use, reproduce, prepare derivative  *
 *    works of, publicly display, publicly perform, sublicense, and          *
 *    distribute your contributions                                          *
 * 3. You represent that you have the legal right to make your contributions *
 *    and that the contributions do not infringe any third-party rights.     *
 * 4. Rick Hadsall and Elwynor Technologies are not obligated to incorporate *
 *    any contributions into the project.                                    *
 * 5. This project is licensed under the AGPL v3, and any derivative works   *
 *    must also be licensed under the AGPL v3.                               *
 * 6. If you create an entirely new project (a fork) based on this work, it  *
 *    must also be licensed under the AGPL v3, you assign all right, title,  *
 *    and interest, including all copyrights, in and to your contributions   *
 *    to Rick Hadsall and Elwynor Technologies, and you must include these   *
 *    additional terms in your project's LICENSE file(s).                    *
 *                                                                           *
 * By contributing to this project, you agree to these terms.                *
 *                                                                           *
****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "elwafk.h"

#define MAXTERMS 256                                            /* number of possible users */
#define ON 1                                                    /* set "Busy" flag on */
#define OFF 0                                                   /* set "Busy" flag off */

VOID EXPORT init__elwafk( VOID );                               /* module initializer */
GBOOL afklog( VOID );                                           /* user log-on routine */
GBOOL afkinput( VOID );                                         /* module user input handler */
VOID clsafk( VOID );                                            /* BBS shutdown routine */
INT globafk( VOID );                                            /* global check user busy */
static VOID getuserid( CHAR *id, INT startarg, INT endarg );    /* returns a user-id from margv[startarg] to margv[endarg] */
static INT uidsearch( CHAR *uid );                              /* returns 0 if no such user, 1 if found, and 2 if multiple possibilities */
static VOID setbusy( struct user *uptr, INT on );               /* sets uptr->userid "Busy" or not "Busy" */
static INT isbusy( struct user *uptr );                         /* returns 1 if uptr->userid is "Busy", 0 if not */

struct module afkmod={
	"",                                                     /* name used to refer to this module */
	afklog,                                                 /* user logon supplemental routine */
	afkinput,                                               /* input routine if selected */
	dfsthn,                                                 /* status-input routine if selected */
	NULL,                                                   /* "injoth" routine for this module */
	NULL,                                                   /* user logoff supplemental routine */
	NULL,                                                   /* hangup (lost carrier) routine */
	NULL,                                                   /* midnight cleanup routine */
	NULL,                                                   /* delete-account routine */
	clsafk,                                                 /* finish-up (sys shutdown) routine */
	};

INT afkstat;                                              /* the module state no. */
HMCVFILE afkmbk;                                          /* module message file pointer */

CHAR *afkstr;                                             /* string to use away notify commands */
CHAR *afkkey;                                             /* key users must have to use notifier */
CHAR isafk[MAXTERMS];                                     /* 1 if isafk[usrnum] is away, 0 if not */
INT savestate[MAXTERMS];                                  /* save usrptr->state here if changelocation */
INT savesubstate[MAXTERMS];                               /* save usrptr->substt here if changelocation */
INT saveclass[MAXTERMS];
CHAR *settings[]={ "at", "away from" };                   /* text to display for user's isafk[] setting */
CHAR *teleconfname;                                       /* the name of the Teleconference module */
INT doactions;                                            /* 1 if do Teleconference actions with afk commands, 0 if not */
CHAR *brbstr;                                             /* the name of the Teleconference action for away from keyboard */
CHAR *bakstr;                                             /* the name of the Teleconference action for back at keyboard */
INT changelocation;                                       /* 1 if change user's location while AFK, 0 if not */
CHAR *afklocation;                                        /* user's location if change while AFK */
INT togglebusy;                                           /* 1 if toggle users "Busy" when AFK and BAK, 0 if not */
UCHAR dobusy[MAXTERMS];                                   /* 1 if turn user's "Busy" OFF when BAK */

CHAR *AFK_NAME="AFK";                                     /* the module name */
CHAR *AFK_VERSION="1.5";                                  /* program version */

VOID EXPORT init__elwafk( VOID )                   /* the module initialization routine */
{
  afkmbk=opnmsg("elwafk.mcv");

  stzcpy(afkmod.descrp, stgopt(AFKLOC), MNMSIZ);
  afkstat=register_module(&afkmod);

  globalcmd(globafk);

  afkstr=stgopt(AFKSTR);
  afkkey=stgopt(AFKKEY);
  doactions=ynopt(TELAFK);
  teleconfname=stgopt(TELNAM);
  brbstr=stgopt(AFKCOM);
  bakstr=stgopt(BAKCOM);
  changelocation=ynopt(CHGLOC);
  afklocation=stgopt(AFKLOC);
  togglebusy=ynopt(SETBUSY);

  shocst(spr("ELW AFK v%s",AFK_VERSION),"(C) Copyright 2024 Elwynor Technologies - www.elwynor.com");
}

GBOOL afklog( VOID )                    /* user log-on routine */
{
  if ( haskey(afkkey) ) {       /* display help on module */
    setmbk(afkmbk);
    prfmsg(AFKALT, AFK_VERSION, afkstr);
    outprf(usrnum);
    rstmbk();
  }
  isafk[usrnum]=0;
  dobusy[usrnum]=0;
  return(FALSE);
}

GBOOL afkinput( VOID )                  /* module user input handler */
{       /* remind user how to get out of here */
  setmbk(afkmbk);
  prfmsg(TOLEAVE, afkstr);
  outprf(usrnum);
  rstmbk();
  return(TRUE);
}

VOID clsafk( VOID )                     /* BBS shutdown routine */
{
  clsmsg(afkmbk);
}

INT globafk( VOID )                     /* global check away from keyboard */
{
  CHAR afkid[INPSIZ];             /* user-id to check for validity and busy */
  INT found;                      /* result of uidsearch; 0 if not found, 1 if ok, 2 if more than 1 possible user-id */
  INT telstate;                   /* module number of Teleconference module */

  if ( margc > 0 && sameas( margv[0], afkstr ) )	{       /* user is asking for this module */
    if ( !haskey(afkkey) )	{       /* can't use command, so leave module */
      setmbk(afkmbk);
      prfmsg(NOAFK);
      outprf(usrnum);
      rstmbk();
      return(1);
    }
    if ( margc == 2 && sameas( margv[1], "?" ) )	{       /* user wants help */
      setmbk(afkmbk);
      prfmsg(AFKHLP, afkstr, afkstr, settings[isafk[usrnum]]);
      outprf(usrnum);
      rstmbk();
      return(1);
    }
    if ( margc == 1 )	{       /* toggle user's current setting */
      isafk[usrnum]^=1;
      setmbk(afkmbk);
      prfmsg(YOURAFK, settings[isafk[usrnum]]);
      outprf(usrnum);
      rstmbk();
      if ( togglebusy )	{       /* set user "Busy" and then restore */
        if ( isafk[usrnum] )	{       /* user is leaving keyboard */
          if ( !isbusy(usrptr) )	{       /* set busy and remember to toggle when back */
            setbusy(usrptr, (INT)ON);
            dobusy[usrnum]=1;
          }
        } else	{       /* user is back at keyboard */
          if ( dobusy[usrnum] ) {       /* turn "Busy" off */
            setbusy(usrptr, (INT)OFF);
            dobusy[usrnum]=0;
          }
        }
      }
			
      telstate=findmod(teleconfname);

      if ( changelocation ) {       /* check that we've changed location */
        if ( isafk[usrnum] )	{       /* user has left keyboard */
          if (doactions && usrptr->state==telstate) {
            strcpy(input, brbstr);
            parsin();
            (*(VOIDFUNC *)(module[telstate]->sttrou))();
          }
          savestate[usrnum]=usrptr->state;
          savesubstate[usrnum]=usrptr->substt;
          saveclass[usrnum]=usrptr->usrcls;
          usrptr->state=afkstat;
          usrptr->substt=1;
        } else	{       /* user has returned */
          usrptr->state=savestate[usrnum];
          usrptr->substt=savesubstate[usrnum];
          usrptr->usrcls=saveclass[usrnum];
          if (doactions && usrptr->state==telstate) {
            strcpy(input, bakstr);
            parsin();
            (*(VOIDFUNC *)(module[telstate]->sttrou))();
          }
        }
      }
      return(1);
    }
		
    getuserid( afkid, 1, margc-1 );
    found=uidsearch(afkid);
    setmbk(afkmbk);
    switch ( found )	{
      case 0 :        /* no such user on */
        prfmsg(NOTON, afkid);
        break;
      case 1 :        /* found exact person */
        onsys(afkid);          /* set othusp and othuap; we know they are on-line */
        prfmsg(AFKSET, othuap->userid, settings[isafk[othusn]]);
        break;
      case 2 :        /* need to better specify who you mean */
        prfmsg(WHO, afkid, afkstr);
    }
    outprf(usrnum);
    rstmbk();
    return(1);
  }
	
  return(0);
}

static VOID getuserid( CHAR *id, INT startarg, INT endarg )     /* returns a user-id from margv[startarg] to margv[endarg] */
{
  INT idloop;                     /* number of strings to concat together */

  setmem( id, INPSIZ, 0 );
  strcpy( id, margv[startarg] );
  for ( idloop=startarg+1; idloop <= endarg; idloop++ ) {
    strcat( id, " " );
    strcat( id, margv[idloop] );
  }
}

static INT uidsearch( CHAR *uid )                               /* returns 0 if no such user, 1 if found, 2 if more than 1 possibility */
{
  INT match=0;                    /* result of search for matching user-id */
  CHAR possibleid[UIDSIZ];        /* user-id of match, for return in uid */

  for ( othusn=0, othusp=usroff(othusn); othusn < nterms; othusn++, othusp=usroff(othusn) ) {
    othuap=uacoff(othusn);
    if ( !(othusp->flags&INVISB) && (othusp->usrcls >= SUPLON) ) {       /* possible user here */
      if ( sameas( uid, othuap->userid ) ) {       /* exact match, this is the dude */
        strcpy( uid, othuap->userid );
        return(1);
      }
      if ( sameto( uid, othuap->userid ) ) {       /* partial match found */
        if ( !match ) {       /* copy potential user-id for return */
          strcpy( possibleid, othuap->userid );
        } else	{       /* more than one possible match, ERROR */
          return(2);
        }
        match++;
      }
    }
  }
  
  if (match)		       /* 1 match found, so get exact user-id */
    strcpy( uid, possibleid );
		
  return(match);
}

static VOID setbusy(struct user *uptr, INT on)                /* sets uptr->userid "Busy" if on=1, or not "Busy" */
{       
  if (on)       /* turn "Busy" on */
    uptr->flags|=NOINJO;
  else          /* turn "Busy" off */
    uptr->flags&=~NOINJO;
	
}

static INT isbusy(struct user *uptr)                          /* returns 1 if uptr->userid is "Busy", 0 if not */
{
  return( (uptr->flags&NOINJO) != 0L );
}

/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cln_utl.h,v 1.1 2002-05-02 06:10:30 zender Exp $ */

/* Purpose: Calendar utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_cln_utl.h" *//* Calendar utilities */

#ifndef NCO_CLN_UTL_H
#define NCO_CLN_UTL_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */

/* Personal headers */
#include "nco.h" /* NCO definitions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int /* O [nbr] Number of days to end of month */
nd2endm /* [fnc] Compute number of days to end of month */
(const int mth, /* I [mth] Month */
 const int day); /* I [day] Current day */

nco_long /* O [YYMMDD] Date a specified number of days from input date */
newdate /* [fnc] Compute date a specified number of days from input date */
(const nco_long date, /* I [YYMMDD] Date */
 const int day_srt); /* I [day] Days ahead of input date */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CLN_UTL_H */

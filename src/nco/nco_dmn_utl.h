/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_dmn_utl.h,v 1.5 2002-09-03 01:19:54 zender Exp $ */

/* Purpose: Dimension utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_dmn_utl.h" *//* Dimension utilities */

#ifndef NCO_DMN_UTL_H
#define NCO_DMN_UTL_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
nco_dmn_dfn /* [fnc] Define dimensions in output file */
(const char * const fl_nm, /* I [sng] Name of output file */
 const int nc_id, /* I [id] netCDF output file ID */
 dmn_sct ** const dmn, /* I [sct] Dimension structures to be defined in output file */
 const int nbr_dmn); /* I [nbr] Number of dimension structures in structure list */

dmn_sct * /* O [sct] copy of input dimension structure */
nco_dmn_dpl /* [fnc] Duplicate input dimension structure */
(const dmn_sct * const dmn); /* I [sct] Dimension structure to duplicate */

dmn_sct * /* O [sct] Output dimension structure */
nco_dmn_fll /* [fnc] Create and return completed dmn_sct */
(const int nc_id, /* I [id] netCDF input file ID*/
 const int dmn_id, /* I [id] Dimension ID */
 const char * const dmn_nm); /* I [sng] Dimension name */

void
nco_dmn_lmt_mrg /* [fnc] Merge limit structure information into dimension structures */
(dmn_sct ** const dmn, /* I [sct] Dimension structures to modify */
 const int nbr_dmn, /* I [nbr] Number of dimension structures in structure list */
 const lmt_sct * const lmt, /* I [sct] Dimension limit information */
 const int lmt_nbr); /* I [nbr] Number of dimensions with user-specified limits */

nm_id_sct * /* O [sct] Dimension list */
nco_dmn_lst_mk /* [fnc] Attach dimension IDs to dimension list */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const * const dmn_lst_in, /* I [sng] User-specified list of dimension names */
 const int nbr_dmn); /* I [nbr] Total number of dimensions in list */

nm_id_sct * /* O [sct] List of dimensions associated with input variable list */
nco_dmn_lst_ass_var /* [fnc] Create list of all dimensions associated with input variable list */
(const int nc_id, /* I [id] netCDF input-file ID */
 const nm_id_sct * const var, /* I [sct] Variable list */
 const int nbr_var, /* I [nbr] Number of variables in list */
 int * const nbr_dmn); /* O [nbr] Number of dimensions associated with input variable list */

void
nco_dmn_xrf  /* [fnc] Crossreference xrf elements of dimension structures */
(dmn_sct * const dmn_1, /* I/O [sct] Dimension structure */
 dmn_sct * const dmn_2); /* I/O [sct] Dimension structure */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_DMN_UTL_H */

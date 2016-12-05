/* $Header$ */

/* Purpose: Calendar utilities */

/* Copyright (C) 1995--2016 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_cln_utl.h" /* Calendar utilities */

/* Arrays to hold calendar type units */
/* Format: year, month, day, hour, minute, second, origin, offset */
double DATA_360[8]={31104000.0,2592000.0,86400.0,3600.0,60.0,1,0.0,0.0};
double DATA_365[8]={31536000.0,2628000.0,86400.0,3600.0,60.0,1,0.0,0.0};

/* Days in months */
int DAYS_PER_MONTH_360[12]={30,30,30,30,30,30,30,30,30,30,30,30};
int DAYS_PER_MONTH_365[12]={31,28,31,30,31,30,31,31,30,31,30,31};
int DAYS_PER_MONTH_366[12]={31,29,31,30,31,30,31,31,30,31,30,31};

int /* O [nbr] Number of days to end of month */
nco_nd2endm /* [fnc] Compute number of days to end of month */
(const int mth, /* I [mth] Month */
 const int day) /* I [day] Current day */
{
  /* Purpose: Returns number of days to end of month  
     This number added to the input argument day gives last day of month mth
     Original fortran: Brian Eaton cal_util.F:nd2endm()
     C version: Charlie Zender */
  const int mdays[]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int nbr_day_2_mth_end;
  
  if(mth < 1 || mth > 12 || day < 0){
    (void)fprintf(stdout,"%s: ERROR nco_nd2endm() reports mth = %d, day = %d\n",nco_prg_nm_get(),mth,day);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  
  nbr_day_2_mth_end=mdays[mth-1]-day;
  
  return nbr_day_2_mth_end;
} /* nco_nd2endm */

nco_int /* O [YYMMDD] Date a specified number of days from input date */
nco_newdate /* [fnc] Compute date a specified number of days from input date */
(const nco_int date, /* I [YYMMDD] Date */
 const nco_int day_srt) /* I [day] Days ahead of input date */
{
  /* Purpose: Find date a specified number of days (possibly negative) from given date 
     Original fortran: Brian Eaton cal_util.F:newdate()
     C version: Charlie Zender */
  
  /* Local */
  const long mth_day_nbr[]= /* Number of days in each month */
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
     31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  
  long day_nbr_2_eom; /* Days to end of month */
  long day_crr; /* Day of date */
  long day_ncr; /* Running count of days to increment date by */
  long mth_crr; /* Month of date */
  long mth_idx; /* Index */
  long mth_srt; /* Save the initial value of month */
  long mth_tmp; /* Current month as we increment date */
  long yr_crr; /* Year of date */
  
  nco_int date_srt; /* Initial value of date (may change sign) */
  nco_int newdate_YYMMDD; /* New date in YYMMDD format */
  
  if(day_srt == 0L) return date;
  
  date_srt=date;
  yr_crr=date_srt/10000L;
  if(date_srt < 0L) date_srt=-date_srt;
  mth_crr=(date_srt%10000L)/100L;
  mth_srt=mth_crr;
  day_crr=date_srt%100L;
  
  if(day_srt > 0){
    day_ncr=day_srt;
    yr_crr+=day_ncr/365L;
    day_ncr=day_ncr%365L;
    for(mth_idx=mth_srt;mth_idx<=mth_srt+12L;mth_idx++){
      mth_tmp=mth_idx;
      if(mth_idx > 12L) mth_tmp=mth_idx-12L;
      day_nbr_2_eom=(long int)nco_nd2endm(mth_tmp,day_crr);
      if(day_ncr > day_nbr_2_eom){
	mth_crr++;
	if(mth_crr > 12L){
	  mth_crr=1L;
	  yr_crr++;
	} /* end if */
	day_crr=1L;
	day_ncr-=day_nbr_2_eom+1L;
	if(day_ncr == 0L) break;
      }else{
	day_crr=day_crr+day_ncr;
	break;
      } /* end if */
    } /* end loop over mth */
    /* Previous two breaks continue execution here */
  }else if(day_srt < 0L){
    day_ncr=-day_srt;
    yr_crr=yr_crr-day_ncr/365L;
    day_ncr=day_ncr%365L;
    mth_srt=mth_crr;
    for(mth_idx=mth_srt+12L;mth_idx>=mth_srt;mth_idx--){
      if(day_ncr >= day_crr){
	mth_crr--;
	if(mth_crr < 1L){
	  mth_crr=12L;
	  yr_crr--;
	} /* end if */
	day_ncr-=day_crr;
	day_crr=mth_day_nbr[mth_crr-1L];
	if(day_ncr == 0L) break;
      }else{
	day_crr-=day_ncr;
	break;
      } /* end if */
    } /* end loop over mth */
    /* Previous two breaks continue execution here */
  } /* end if */
  
  if(yr_crr >= 0){
    newdate_YYMMDD=yr_crr*10000L+mth_crr*100L+day_crr;
  }else{
    newdate_YYMMDD=-yr_crr*10000L+mth_crr*100L+day_crr;
    newdate_YYMMDD=-newdate_YYMMDD;
  } /* end if */
  
  return newdate_YYMMDD;
} /* end nco_newdate() */


tm_typ /* O [enm] Units type */
nco_cln_get_tm_typ /* Returns time unit type or tm_void if not found */
(const char *ud_sng){ /* I [ptr] Units string  */
  int idx;
  int len; 
  char *lcl_sng;  
  tm_typ rcd_typ;
  
  lcl_sng=strdup(ud_sng);
  
  /* Set initial return type to void and overwrite */
  rcd_typ=tm_void;   
  
  /* Convert to lower case */
  len=strlen(lcl_sng);
  for(idx=0;idx<len;idx++) lcl_sng[idx]=tolower(lcl_sng[idx]);
  
  if(!strcmp(lcl_sng,"year") || !strcmp(lcl_sng,"years")) rcd_typ=tm_year;
  else if(!strcmp(lcl_sng,"month") || !strcmp(lcl_sng,"months")) rcd_typ=tm_month;
  else if(!strcmp(lcl_sng,"day") || !strcmp(lcl_sng,"days")) rcd_typ=tm_day;
  else if(!strcmp(lcl_sng,"hour") || !strcmp(lcl_sng,"hours")) rcd_typ=tm_hour;
  else if(!strcmp(lcl_sng,"min") || !strcmp(lcl_sng,"mins") || !strcmp(lcl_sng,"minute") || !strcmp(lcl_sng,"minutes")) rcd_typ=tm_min;
  else if(!strcmp(lcl_sng,"sec") || !strcmp(lcl_sng,"secs") || !strcmp(lcl_sng,"second") || !strcmp(lcl_sng,"seconds")) rcd_typ=tm_sec;
  
  if(lcl_sng) lcl_sng=(char *)nco_free(lcl_sng);
  return rcd_typ;
} /* end nco_cln_get_tm_typ() */

nco_cln_typ /* [enm] Calendar type */
nco_cln_get_cln_typ /* [fnc] Determine calendar type or cln_nil if not found */
(const char *ud_sng) /* I [ptr] units string  */
{
  int idx;
  int len; 
  char *lcl_sng;  
  nco_cln_typ rcd_typ;
  
  if(!ud_sng) return cln_nil;
  
  lcl_sng=strdup(ud_sng);
  
  /* Set initial return type to void then overwrite */
  rcd_typ=cln_nil;
  
  /* Convert to lower case */
  len=strlen(lcl_sng);
  for(idx=0;idx<len;idx++) lcl_sng[idx]=tolower(lcl_sng[idx]);
  
  if(strstr(lcl_sng,"standard")) rcd_typ=cln_std;
  else if(strstr(lcl_sng,"gregorian") || strstr(lcl_sng,"proleptic_gregorian")) rcd_typ=cln_grg;
  else if(strstr(lcl_sng,"julian")) rcd_typ=cln_jul;
  else if(strstr(lcl_sng,"360_day")) rcd_typ=cln_360;
  else if(strstr(lcl_sng,"noleap") || strstr(lcl_sng,"365_day")) rcd_typ=cln_365;
  else if(strstr(lcl_sng,"all_leap") || strstr(lcl_sng,"366_day")) rcd_typ=cln_366;

  lcl_sng=(char *)nco_free(lcl_sng);
  
  return rcd_typ;
} /* end nco_cln_get_cln_typ() */

int /* O [nbr] Number of days */
nco_cln_days_in_year_prior_to_given_month /* [fnc] Number of days in year prior to month */
(nco_cln_typ lmt_cln, /* [enm] Calendar type */
 int mth_idx) /* I [idx] Month (1-based counting, December == 12) */
{ 
  int *days=NULL_CEWI;
  int idx;
  int idays=0;
  
  switch(lmt_cln){
  case cln_360:
    days=DAYS_PER_MONTH_360;
    break; 
  case cln_365:
    days=DAYS_PER_MONTH_365;
    break; 
  case cln_366: 
    days=DAYS_PER_MONTH_366;
    break;  
  case cln_std:
  case cln_grg:
  case cln_jul:
  case cln_nil:
    break;
  } /* end switch */
  
  mth_idx--;
  
  for(idx=0;idx<mth_idx;idx++) idays+=days[idx];
  
  return idays;
} /* end nco_cln_days_in_year_prior_to_given_month() */

void
nco_cln_pop_val /* [fnc] Calculate value in cln_sct */ 
(tm_cln_sct *cln_sct) /* I/O [ptr] structure */
{
  /* Purpose: Populate values in calendar structure */
  double *data;
  
  switch(cln_sct->sc_cln){
  case cln_360:
    data=DATA_360;
    cln_sct->value=data[0]*(cln_sct->year-1)+
      data[1]*(cln_sct->month-1)+
      data[2]*(cln_sct->day-1)+
      data[3]*cln_sct->hour+
      data[4]*cln_sct->min+
      data[5]*(double)cln_sct->sec;
    break; 
  case cln_365:  
    data=DATA_365;    
    cln_sct->value=data[0]*(cln_sct->year-1)+
      data[2]*nco_cln_days_in_year_prior_to_given_month(cln_365,cln_sct->month)+
      data[2]*(cln_sct->day-1)+
      data[3]*cln_sct->hour+
      data[4]*cln_sct->min+
      data[5]*(double)cln_sct->sec;
    break;
  case cln_366:
    break;
  case cln_std:
  case cln_grg:
  case cln_jul:
  case cln_nil:
    break;
  } /* end switch */
  
  return;
} /* end nco_cln_pop_val() */

double /* O [dbl] relative time */
nco_cln_rel_val
(double offset, /* I [dbl] time in base units */
 nco_cln_typ lmt_cln, /* I [enm] Calendar type */ 
 tm_typ bs_tm_typ) /* I [enm] Time units */
{
  double *data=NULL_CEWI;
  double scl=double_CEWI;
  
  switch(lmt_cln) {
  case cln_360:
    data=DATA_360;    
    break; 
  case cln_365:  
    data=DATA_365;    
    break;
  case cln_366:
    break;
  case cln_std:
  case cln_grg:
  case cln_jul:
  case cln_nil:
    break;
  }
  
  /* Switch for type */
  switch(bs_tm_typ){ 
  case tm_year:    
    scl=data[0];
    break;   
  case tm_month:
    scl=data[1];
    break;   
  case tm_day:
    scl=data[2];
    break;   
  case tm_hour:
    scl=data[3];
    break;   
  case tm_min:
    scl=data[4];
    break;   
  case tm_sec:
  case tm_void: 
    scl=data[5];
    break;     
  } /* end switch */ 
  
  return offset/scl;
} /* end nco_cln_rel_val() */


int /* O [flg] String is calendar date */
nco_cln_chk_tm /* [fnc] Is string a UDUnits-compatible calendar format, e.g., "PERIOD since REFERENCE_DATE" */
(const char *unit_sng) /* I [sng] Units string */
{
  /* Purpose:
     Determine whether the string is a UDUnits-compatible calendar format, e.g., "PERIOD since REFERENCE_DATE" */

  /* Does string contain date keyword? */
  if(strcasestr(unit_sng," from ") || strcasestr(unit_sng," since ") || strcasestr(unit_sng," after ")) return True; else return False;

} /* end nco_cln_chk_tm() */

#ifndef ENABLE_UDUNITS

/* stub functions so can compile without UDUNITS2 */

int    /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_var_dff( /* [fnc] difference between two co-ordinate units */
const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
nco_cln_typ lmt_cln,    /* I [enum] Calendar type of coordinate var */ 
double *dval,           /* I/O [dbl] var values modified */
var_sct *var)           /* I/O [var_sct] var values modified */
{

  return NCO_NOERR;

}
int   /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_org(   /* [fnc] difference between two co-ordinate units */
const char *val_unt_sng, /* I [ptr] input value and  units in the same string */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
nco_cln_typ lmt_cln,    /* I [enum] Calendar type of coordinate var */ 
double *og_val)         /* O [dbl] output value */
{
 
  *og_val=0.0;
  return NCO_NOERR; 
}

#endif

#ifdef ENABLE_UDUNITS
# ifdef HAVE_UDUNITS2_H

/* UDUnits2 routines */
cv_converter*   /* UDUnits converter */
nco_cln_cnv_mk  /* [fnc] UDUnits2 create a custom converter  */
(const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
 const char *fl_bs_sng) /* I [ptr] units attribute string from disk */
{
  const char fnc_nm[]="nco_cln_cnv_mk"; /* [sng] Function name */
  
  cv_converter *ut_cnv; /* UDUnits converter */

  int ut_rcd; /* [enm] UDUnits2 status */
  
  ut_system *ut_sys;
  ut_unit *ut_sct_in; /* [sct] UDUnits structure, input units */
  ut_unit *ut_sct_out; /* [sct] UDUnits structure, output units */
  
  /* When empty, ut_read_xml() uses environment variable UDUNITS2_XML_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) ut_set_error_message_handler(ut_write_to_stderr); else ut_set_error_message_handler(ut_ignore);
  ut_sys=ut_read_xml(NULL);
  if(ut_sys == NULL){
    (void)fprintf(stdout,"%s: %s() failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    return (cv_converter*)NULL; /* Failure */
  } /* end if err */ 
  
  /* Units string to convert from */
  ut_sct_in=ut_parse(ut_sys,fl_unt_sng,UT_ASCII); 
  if(!ut_sct_in){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute \"%s\" has a syntax error\n",fl_unt_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",fl_unt_sng);
    return (cv_converter*)NULL; /* Failure */
  } /* endif coordinate on disk has no units attribute */

  /* Units string to convert to */
  ut_sct_out=ut_parse(ut_sys,fl_bs_sng,UT_ASCII); 
  if(!ut_sct_out){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: Empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute  \"%s\" has a syntax error\n",fl_bs_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",fl_bs_sng);
    return (cv_converter*)NULL; /* Failure */
  } /* endif */

  /* Create converter */
  ut_cnv=ut_get_converter(ut_sct_in,ut_sct_out); /* UDUnits converter */
  if(!ut_cnv){
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"WARNING: One of units, %s or %s, is NULL\n",fl_bs_sng,fl_unt_sng);
    if(ut_rcd == UT_NOT_SAME_SYSTEM) (void)fprintf(stderr,"WARNING: Units %s and %s belong to different unit systems\n",fl_bs_sng,fl_unt_sng);
    if(ut_rcd == UT_MEANINGLESS) (void)fprintf(stderr,"WARNING: Conversion between user-specified unit \"%s\" and file units \"%s\" is meaningless\n",fl_bs_sng,fl_unt_sng);
    return (cv_converter*)NULL; /* Failure */
  } /* endif */

  ut_free(ut_sct_in);
  ut_free(ut_sct_out);
  ut_free_system(ut_sys); /* Free memory taken by UDUnits library */
  // cv_free(ut_cnv);


  return ut_cnv;
}  /* end UDUnits2 nco_cln_cnv_mk() */


int    /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_dff( /* [fnc] difference between two co-ordinate units */
const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
double *og_val)
{           /* I/O [dbl] var values modified */

  cv_converter *ut_cnv=NULL; 
  const char fnc_nm[]="nco_cln_clc_var_dff()"; /* [sng] Function name */
  
  
  /* do nothing if units identical */
  if(strcasecmp(fl_unt_sng,fl_bs_sng)==0)
    return NCO_NOERR;

  /* Convert */
  ut_cnv=nco_cln_cnv_mk(fl_unt_sng, fl_bs_sng);

  if(ut_cnv != NULL)
     og_val[0]=cv_convert_double(ut_cnv,og_val[0]);                      
  else
     return NCO_ERR;     

 cv_free(ut_cnv);  

 return NCO_NOERR;          
}  


int    /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_var_dff( /* [fnc] difference between two co-ordinate units */
const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
var_sct *var)
{           /* I/O [dbl] var values modified */

  size_t sz;
  size_t idx;    
  double *dp;  
  ptr_unn op1;    

  cv_converter *ut_cnv=NULL; 
  const char fnc_nm[]="nco_cln_clc_var_dff()"; /* [sng] Function name */
  
  
  /* do nothing if units identical */
  if(strcasecmp(fl_unt_sng,fl_bs_sng)==0)
    return NCO_NOERR;

  /* Convert */
  ut_cnv=nco_cln_cnv_mk(fl_unt_sng, fl_bs_sng);

  if(ut_cnv == NULL)
      return NCO_ERR;     

  sz=var->sz;  
                
  op1=var->val;
  dp=op1.dp;     
  (void)cast_void_nctype(NC_DOUBLE,&op1);

  if(var->has_mss_val)
  {
      double mss_dbl=var->mss_val.dp[0]; 
      for(idx=0; idx<sz; idx++)
	if( dp[idx] != mss_dbl)   
	  dp[idx]=cv_convert_double(ut_cnv,dp[idx]);                      
 } 
 else
 {
     (void)cv_convert_doubles(ut_cnv,dp,sz,dp);                      
 } 

 cv_free(ut_cnv);  
 (void)cast_nctype_void(NC_DOUBLE,&op1);

 return NCO_NOERR;
                
}  


int    /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_var_dff( /* [fnc] difference between two co-ordinate units */
const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
nco_cln_typ lmt_cln,    /* I [enum] Calendar type of coordinate var */ 
double *og_val,           /* I/O [dbl] var values modified -can be NULL */
var_sct *var)           /* I/O [var_sct] var values modified - can be NULL  */
{
  int rcd;
  int is_date;
  const char fnc_nm[]="nco_cln_clc_dv_dff()"; /* [sng] Function name */
  
  /* do nothing if units identical */
  if(!strcasecmp(fl_unt_sng,fl_bs_sng))
    return NCO_NOERR;

  
  (void)fprintf(stderr,"%s: nco_cln_dbl_var_dff() reports unt_sng=%s bs_sng=%s calendar=%d\n",nco_prg_nm_get(),fl_unt_sng,fl_bs_sng,lmt_cln);

  /* see if target units is of the form  "units since date-string" */
  is_date = nco_cln_chk_tm(fl_bs_sng);

  /* use custom time functions if irregular calendar */
  if(is_date && (lmt_cln==cln_360 || lmt_cln==cln_365) )
    rcd=nco_cln_clc_tm( fl_unt_sng, fl_bs_sng,lmt_cln, og_val, var);  

  else if(og_val != (double*)NULL) 
     rcd=nco_cln_clc_dbl_dff(fl_unt_sng, fl_bs_sng,og_val);  

  else if(var != (var_sct*)NULL)
    rcd=nco_cln_clc_var_dff(fl_unt_sng, fl_bs_sng,var);

  

  return rcd;

}  /* end UDUnits2 nco_cln_clc_dff() */



int   /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_org(   /* [fnc] difference between two co-ordinate units */
const char *val_unt_sng, /* I [ptr] input value and  units in the same string */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
nco_cln_typ lmt_cln,    /* I [enum] Calendar type of coordinate var */ 
double *og_val)         /* O [dbl] output value */
{
  int is_date;  /* set to true if date/time unit */
  int rcd;
  int month;
  int year;

  char lcl_unt_sng[200];
  const char fnc_nm[]="nco_cln_dbl_org"; /* [sng] Function name */
  
  double dval;

  rcd=0;  

  dval=0.0;

  is_date = nco_cln_chk_tm(fl_bs_sng);

  lcl_unt_sng[0]='\0';
  
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) 
       (void)fprintf(stderr,"%s: INFO %s: reports unt_sng=%s bs_sng=%s calendar=%d\n",nco_prg_nm_get(),fnc_nm,val_unt_sng,fl_bs_sng,lmt_cln);

  
    /* Does fl_unt_sng look like a regular timestamp? */ 
  if(is_date && sscanf(val_unt_sng,"%d-%d",&year,&month) == 2)
  {
    /* udunits requires  this prefix that it can recognize a raw-datetime string */  
    strcpy(lcl_unt_sng,"s@");
    strcat(lcl_unt_sng,val_unt_sng);
  }
  else
  {
    /* Regular conversion of fl_unt_sng of form <double_value units>, e.g., '10 inches', '100 ft'  '10 days since 1970-01-01' */
    char *ptr=(char*)NULL;
        
    dval=strtod(val_unt_sng, &ptr);  
        
    if(ptr==val_unt_sng || strlen(++ptr)<1)
    {
      (void)fprintf(stderr, "%s: INFO %s() reports input string must be of the form \"value unit\" got the string \"%s\"\n",nco_prg_nm_get(),fnc_nm,val_unt_sng);
      nco_exit(EXIT_FAILURE);           
    }
    strcpy(lcl_unt_sng,ptr); 
  }

  /* use custom time functions if irregular calendar */
  if(is_date && (lmt_cln==cln_360 || lmt_cln==cln_365) )
    rcd=nco_cln_clc_tm( lcl_unt_sng, fl_bs_sng,lmt_cln,&dval, (var_sct*)NULL);  
  else
    rcd=nco_cln_clc_dbl_dff(lcl_unt_sng, fl_bs_sng,&dval);     
	       	     

  /* only copy over if successfull */ 
  if(rcd==NCO_NOERR)
     *og_val=dval;
    
  return rcd;        
}


/* This function is only called if the target units in fl_bs_sng of the form "value unit since date-stamp" 
   and the calendar type is cln_360 or cln_365.
   either "var" is NULL and there is a single value to process *og_val 
   or var is initialized and  og_val is NULL  */
int /* [rcd] Successful conversion returns NCO_NOERR */
nco_cln_clc_tm /* [fnc] Difference between two coordinate units */
(const char *fl_unt_sng, /* I [ptr] Units attribute string from disk */
 const char *fl_bs_sng, /* I [ptr] Units attribute string from disk */
 nco_cln_typ lmt_cln, /* [enum] Calendar type of coordinate variable */ 
 double *og_val, /* I/O [ptr] */
 var_sct *var){  /* I/O [ptr] */  
  int rcd;
  int year;
  int month;
  char *lcl_unt_sng;
  /* 20141230 figure out better length */
  char tmp_sng[100];
  double crr_val;
  
  tm_typ bs_tm_typ; /* enum for units type in fl_bs_sng */
  tm_cln_sct unt_cln_sct;
  tm_cln_sct bs_cln_sct;
  
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: nco_cln_clc_tm() reports unt_sng=%s bs_sng=%s\n",nco_prg_nm_get(),fl_unt_sng,fl_bs_sng);

  /* blow out if bad cln type */
  if(lmt_cln != cln_360 &&  lmt_cln != cln_365)
  {
    (void)fprintf(stderr,"%s: nco_cln_clc_tm() has been called with wrong calander types - only cln_365 and cln_360 allowed\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE); 
  }       
  

  /* Obtain units type from fl_bs_sng */
  if(sscanf(fl_bs_sng,"%s",tmp_sng) != 1) return NCO_ERR;
  
  bs_tm_typ=nco_cln_get_tm_typ(tmp_sng);  
  
  /* Assume non-standard calendar */ 
  if(nco_cln_prs_tm(fl_unt_sng,&unt_cln_sct) == NCO_ERR) return NCO_ERR;
  if(nco_cln_prs_tm(fl_bs_sng,&bs_cln_sct) == NCO_ERR) return NCO_ERR;
  
  unt_cln_sct.sc_typ=bs_tm_typ;
  bs_cln_sct.sc_typ=bs_tm_typ;
  
  unt_cln_sct.sc_cln=lmt_cln;
  bs_cln_sct.sc_cln=lmt_cln; 
  (void)nco_cln_pop_val(&unt_cln_sct);
  (void)nco_cln_pop_val(&bs_cln_sct);
  
  crr_val=nco_cln_rel_val(unt_cln_sct.value-bs_cln_sct.value,lmt_cln,bs_tm_typ);                 
  
  if(og_val)
  {   
    *og_val+=crr_val;     
  }
  else if(var)
  {
    size_t sz;
    size_t idx;
    double *dp;
    ptr_unn op1;    

    (void)cast_void_nctype(NC_DOUBLE,&op1);

    if(var->has_mss_val)
    {
      double mss_dbl=var->mss_val.dp[0]; 
      for(idx=0; idx<sz; idx++)
	 if( dp[idx] != mss_dbl)   
	   dp[idx]+=crr_val;                      
   } 
   else
   {
      for(idx=0; idx<sz; idx++)
	  dp[idx]+=crr_val;                      
   } 

   (void)cast_nctype_void(NC_DOUBLE,&op1);

  }
  lcl_unt_sng=(char *)nco_free(lcl_unt_sng);
  
  return NCO_NOERR;
} /* end nco_cln_clc_tm() */



int /* [rcd] Successful conversion returns NCO_NOERR */
nco_cln_prs_tm /* UDUnits2 Extract time stamp from parsed UDUnits string */
(const char *unt_sng, /* I [ptr] units attribute string */
 tm_cln_sct *tm_in) /* O [sct] Time structure to be populated */
{
  const char fnc_nm[]="nco_cln_prs_tm()"; /* [sng] Function name */

  /* 20141230: fxm figure out a better length */
  char bfr[200];

  char *dt_sng;

  int ut_rcd; /* [enm] UDUnits2 status */

  ut_system *ut_sys;
  ut_unit *ut_sct_in; /* UDUnits structure, input units */

  /* When empty, ut_read_xml() uses environment variable UDUNITS2_XML_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) ut_set_error_message_handler(ut_write_to_stderr); else ut_set_error_message_handler(ut_ignore);
  ut_sys=ut_read_xml(NULL);
  if(ut_sys == NULL){
    (void)fprintf(stdout,"%s: %s failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    return NCO_ERR; /* Failure */
  } /* end if err */ 

  /* Units string to convert from */
  ut_sct_in=ut_parse(ut_sys,unt_sng,UT_ASCII); 
  if(ut_sct_in == NULL){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX)  (void)fprintf(stderr,"ERROR: units attribute \"%s\" has a syntax error\n",unt_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",unt_sng);

    return NCO_ERR; /* Failure */
  } /* endif coordinate on disk has no units attribute */

  /* Print timestamp to buffer in standard, dependable format */
  ut_format(ut_sct_in,bfr,sizeof(bfr),UT_ASCII|UT_NAMES);

  /* Extract parsed time units from print string (kludgy)
     20141230 change to using ut_decode_time() instead? */
  dt_sng=strstr(bfr,"since");  
  sscanf(dt_sng,"%*s %d-%d-%d %d:%d:%f",&tm_in->year,&tm_in->month,&tm_in->day,&tm_in->hour,&tm_in->min,&tm_in->sec);

  ut_free_system(ut_sys); /* Free memory taken by UDUnits library */
  ut_free(ut_sct_in);

  return NCO_NOERR;
} /* end UDUnits2 nco_cln_prs_tm() */


int /* [rcd] Return code */
nco_cln_sng_rbs /* [fnc] Rebase calendar string for legibility */
(const ptr_unn val, /* I [sct] Value to rebase */
 const long val_idx, /* I [idx] Index into 1-D array of values */
 const nc_type val_typ, /* I [enm] Value type */
 const char *unit_sng, /* I [sng] Units string */
 char *lgb_sng) /* O [sng] Legible version of input string */
{
  /* Purpose: Rebase calendar string for legibility
     Assumptions: Input units string unit_sng is a calendar date, i.e., contains "from", "since", or "after"

     ncdump handles this in nctime0.c
     dumplib.c/nctime_val_tostring() by Dave Allured, NOAA
     cdRel2Iso() from CDMS by Bob Drach, LLNL
     cdParseRelunits() from CDMS by Bob Drach, LLNL */

  const char fnc_nm[]="nco_cln_sng_rbs()"; /* [sng] Function name */
  
  double val_dbl; /* [day] Calendar offset converted to double */

  int ut_rcd; /* [enm] UDUnits2 status */
  
  ut_system *ut_sys;
  ut_unit *ut_sct_in; /* [sct] UDUnits structure, input units */
  ut_unit *ut_sct_out; /* [sct] UDUnits structure, output units */

  /* Quick return if units DNE */
  if(!unit_sng) return NCO_NOERR;
  
  /* When empty, ut_read_xml() uses environment variable UDUNITS2_XML_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) ut_set_error_message_handler(ut_write_to_stderr); else ut_set_error_message_handler(ut_ignore);
  ut_sys=ut_read_xml(NULL);
  if(!ut_sys){
    (void)fprintf(stdout,"%s: %s() failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    return NCO_ERR; /* Failure */
  } /* end if err */ 

  /* Units string containing calendar origin converted to UDUnit structure */
  ut_sct_in=ut_parse(ut_sys,unit_sng,UT_ASCII); 
  if(!ut_sct_in){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute \"%s\" has a syntax error\n",unit_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",unit_sng);
    return NCO_ERR; /* Failure */
  } /* endif coordinate on disk has no units attribute */

  /* Convert time since calendar origin to double */
  val_dbl=ptr_unn_2_scl_dbl(val,val_typ); 
  
  /* Units string to convert to */
  ut_sct_out=ut_offset(ut_sct_in,val_dbl);
  if(!ut_sct_out){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: Empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute  \"%s\" has a syntax error\n",unit_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",unit_sng);
    return NCO_ERR; /* Failure */
  } /* endif */

  val_dbl+=0*val_idx; /* CEWI */

  ut_free(ut_sct_in);
  ut_free(ut_sct_out);
  ut_free_system(ut_sys); /* Free memory taken by UDUnits library */


  lgb_sng[0]='\0'; /* CEWI */

  return NCO_NOERR;

} /* end nco_cln_sng_rbs() */








#endif  /* HAVE_UDUNITS2_H */
#endif  /* ENABLE_UDUNITS */
/* End UDUnits-related routines*/
 

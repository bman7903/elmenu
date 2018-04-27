#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>


#define MBF 1024
#define SBF 128


struct fcz {
  char* ftyp;  // type
  char* fnme;  // name
  char* fste;  // state
  char* fip4;  // ipv4 address
  char* fmac;  // mac
  char* fflt;  // default
} fwifi[10],feth[10],fbrg[10],ftap[10],fspc[10];


struct fcz *w_fcz;
struct fcz *e_fcz;
struct fcz *b_fcz;
struct fcz *t_fcz;
struct fcz *s_fcz;


char*
strip_copy
  (s)
     char *s;
{
  char *p = malloc(strlen(s) + 1);
  if (p) {
     char *p2 = p;

     while (*s != '\0') {
       if (*s != '\t' && *s != '\n') {

          *p2++ = *s++;
       } else {

          ++s;
       }
     } *p2 = '\0';

  }
  return p;
}


char*
retln
   (fp,pbf)
     char* fp;                  // file path
     char pbf[];                // ret buff
{
  // opens file, reads 1 line, returns that line

  FILE *fd = fopen(fp, "r");
  char buffer[255];

  while
     ( fgets(buffer, 255, (FILE*) fd) )
       strcat(pbf,buffer);

  fclose(fd);
  strcat(pbf,"\0");
  pbf = strip_copy(pbf);
  return pbf;
}


char*
pget
   (cmd,bfr)
     char *cmd;
     char bfr[];
{
  // proc cmd

  char buf[SBF];
  FILE *fp;

  if
     ( (fp = popen(cmd, "r") ) == NULL )
       return "Error";
  while
     ( fgets (buf, SBF, fp) != NULL )
       strcat(bfr,buf);
  if
     (pclose(fp))
       return "Error";

  return bfr;
}



char*
eth2ip
   (fce)
     char* fce;
{
  // translates interface name to ipv4
  int fd;
  struct ifreq ifr;

  char face[20] = "";  // we do not want to translate in place
  strcat(face,fce);
  strcat(face,"\0");

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name, face, IFNAMSIZ-1);
  ioctl(fd, SIOCGIFADDR, &ifr);
  close(fd);

  return  inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}


char*
listdir
   (fbf,fdr)
     char fbf[];  // return buff
     char* fdr;   // dir
{
  // lists dir, returns items

  int i=0;
  DIR *d;
  struct dirent *dir;

  d = opendir(fdr);
  if (d) {

    while ((dir = readdir(d)) != NULL) {
       // skip the first two as they are '.','..' pointers

       if ( i > 1 ) {
         const char* fpth = dir->d_name;
         strcat(fbf, fpth);
         strcat(fbf, "\n");
       } i++;

     } closedir(d);

  } return fbf;
}


int
dflt
   (fce)
     char* fce;
{
  char bfr[4096] = "";
  char* route  = pget("route",bfr);
  char *token  = "default";
  char ** res  = NULL;
  char * p     = strtok (route, "\n");
  int n_spaces = 0, i;
  while (p) {
     if ( strstr(p, token) ) { // too many secrets
       res = realloc(res, sizeof(char*) * ++n_spaces);
       if (res == NULL)
          exit (-1);
       res[n_spaces-1] = malloc( sizeof(char) * strlen(p) );
       strcpy(res[n_spaces-1],p);

     } p = strtok(NULL, "\n");
  }
  res = realloc (res, sizeof (char*) * (n_spaces+1));
  res[n_spaces] = '\0';
  // add +1 to n_spaces to append (null) to array
  for (i = 0; i < (n_spaces); ++i) {
     char* itm = res[i];
     if (strstr (itm,  fce) != NULL) return 1;
     return 0; // we sould have returned answer by here
     free(res[i]);
  }
  free(res);
  return 0;
}


char*
itype
   (idir,bdr)
     char* idir;        // dir list
     char* bdr;         // base dir
{
  if (strstr (bdr,  "lo")  != NULL) return "SPECIAL";
  if (strstr (bdr,  "sit") != NULL) return "SPECIAL";
  if (strstr (bdr,  "dum") != NULL) return "SPECIAL";
  if (strstr (bdr,  "eth") != NULL) return "ETH";
  if (strstr (bdr,  "enp") != NULL) return "ETH";
  char *token = "hidden";  // hide this interface
  char ** res  = NULL;
  char * p = strtok (idir, "\n");
  int n_spaces = 0, i;
  while (p) {
     if ( ! strstr(p, token) ) { // too many secrets

       res = realloc(res,sizeof(char*)*++n_spaces);
       if (res == NULL)
          exit (-1);
       res[n_spaces-1] = malloc(sizeof(char)*strlen(p));
       strcpy(res[n_spaces-1],p);
     } p = strtok(NULL, "\n");
  }
  res = realloc (res, sizeof (char*) * (n_spaces+1));
  res[n_spaces] = '\0';
  for (i = 0; i < (n_spaces); ++i) {
     char* itm = res[i];
     if (strstr (itm,  "80211") != NULL) return "WIFI";
     if (strstr (itm,    "tun") != NULL) return "TAP";
     if (strstr (itm, "bridge") != NULL) return "BRIDGE";
     free(res[i]);
  }

  free(res);
  return "UKNOWN"; // catch all for special
}


void
netinf( void )
{

  int k=0;
  DIR *d;

  struct dirent *dir;
  char* fdr = "/sys/class/net";

  w_fcz = fwifi;
  e_fcz = feth;
  b_fcz = fbrg;
  t_fcz = ftap;
  s_fcz = fspc;


  d = opendir(fdr);
  if (d) {
     while ((dir = readdir(d)) != NULL) {

       if ( k > 1 ) {
          int i;
          int dft;
          char* ist = "";
          char rln[255] = "";  // line buff
          char mln[255] = "";  // mac buff
          char upn[255] = "";  // up buff
          char ipt[255] = "";  // face date
          char iup[255] = "";  // opersate
          char adr[255] = "";  // address
          char lbf[4096] = "";
          char* lflt  = "no";	// default
          char* lip4  = "0.0.0.0";      // ipv4
          char* ltype = "SPECIAL";	// category
          char* lste  = "down";		// state
          char lmac[18];		// mac

          char* lnme = dir->d_name;	// interface

          strcat(ipt, fdr);
          strcat(ipt, "/");
          strcat(ipt, lnme);
          printf("ipt %s\n",ipt);

          strcat(iup, ipt);
          strcat(iup, "/");
          strcat(iup, "operstate");
          strcat(iup, "\0");

          strcat(adr,ipt);
          strcat(adr,"/address");
          strcat(adr,"\0");
          strcat(ipt,"\0");

          lip4 = eth2ip( lnme );

          char ncp[20] = "";
          strcat(ncp,lnme);
          strcat(ncp,"\0");

          dft = dflt( ncp );    // default
          if ( dft > 0 )  lflt = "yes";

          // list sysfs content of network interface
          ist   = listdir( rln, ipt);
          ltype = itype( ist, ipt ); // type
          lste = retln( iup, upn );  // state

          // mac
          int fd2;
          struct ifreq ifr;
          unsigned char *mac = NULL;
          char ma[18];
          memset(&ifr, 0, sizeof(ifr));

          fd2 = socket(AF_INET, SOCK_DGRAM, 0);

          ifr.ifr_addr.sa_family = AF_INET;
          strncpy(ifr.ifr_name , lnme , IFNAMSIZ-1);

          if (0 == ioctl(fd2, SIOCGIFHWADDR, &ifr)) {
             mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
             sprintf( ma, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\0", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
          }
          close(fd2);

         /*  printf("mac %s\n", ma);
          printf("state %s\n", lste);
          printf("type %s\n", ltype);
          printf("default %s\n", lflt);
          printf("ip4 is %s\n", lip4);
          printf("sys face dir is %s\n",ipt);
          printf("face is %s\n\n", lnme); */

          if (strstr (ltype,  "WIFI")    != NULL) {
            i = 0;
            while ( w_fcz[i].fnme   != NULL) { i++; w_fcz++;}
            w_fcz->ftyp   = ltype;
            w_fcz->fnme   = lnme;
            w_fcz->fste   = lste;
            w_fcz->fip4   = lip4;
            w_fcz->fmac   = ma;
            w_fcz->fflt   = lflt;
          }
          if (strstr (ltype,  "ETH")    != NULL) {
            i = 0;
            while ( e_fcz[i].fnme   != NULL) { i++; e_fcz++;}
            e_fcz->ftyp   = ltype;
            e_fcz->fnme   = lnme;
            e_fcz->fste   = lste;
            e_fcz->fip4   = lip4;
            e_fcz->fmac   = ma;
            e_fcz->fflt   = lflt;
          }

          if (strstr (ltype,  "BRIDGE")    != NULL) {
            i = 0;
            while ( b_fcz[i].fnme   != NULL) { i++; b_fcz++;}
            b_fcz->ftyp   = ltype;
            b_fcz->fnme   = lnme;
            b_fcz->fste   = lste;
            b_fcz->fip4   = lip4;
            b_fcz->fmac   = ma;
            b_fcz->fflt   = lflt;
          }

          if (strstr (ltype,  "TAP")    != NULL) {
            i = 0;
            while ( t_fcz[i].fnme   != NULL) { i++; t_fcz++;}
            t_fcz->ftyp   = ltype;
            t_fcz->fnme   = lnme;
            t_fcz->fste   = lste;
            t_fcz->fip4   = lip4;
            t_fcz->fmac   = ma;
            t_fcz->fflt   = lflt;
          }

          if (strstr (ltype,  "SPECIAL")    != NULL) {
            i = 0;
            while ( s_fcz[i].fnme   != NULL) { i++; s_fcz++;}
            s_fcz->ftyp   = ltype;
            s_fcz->fnme   = lnme;
            s_fcz->fste   = lste;
            s_fcz->fip4   = lip4;
            s_fcz->fmac   = ma;
            s_fcz->fflt   = lflt;
          }
       } k++;

     } closedir(d);
  } 

}



/*void main(void)
{
  // parses /sys/class/network for all interfaces and most options
  netinf();
  w_fcz = fwifi;
  char* wfce = w_fcz[0].fnme;
  printf(" winame %s\n", wfce);

  e_fcz = feth;
  char* efce = e_fcz[0].fnme;
  printf(" ethname %s\n", efce);

  b_fcz = fbrg;
  char* bfce = b_fcz[0].fnme;
  printf(" brgname %s\n", bfce);

  t_fcz = ftap;
  char* tfce = t_fcz[0].fnme;
  printf(" tapname %s\n", tfce);

  s_fcz = fspc;
  char* sfce = s_fcz[0].fnme;
  printf(" specialname %s\n", sfce);
  return;
} */


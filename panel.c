//gcc -g panel_example_01.c -o panel_example_01 `pkg-config --cflags --libs elementary`

#include <stdio.h>
#include <stdlib.h>
#include <Elementary.h>
#include "nwork.h"

void
Exit
     (void)
{
  printf("bye bye\n");
  exit(0);
}


void
fling
   (poop)
     int *poop;
{
  printf("flung some %d\n",poop);
}

EAPI_MAIN int
elm_main
   (argc, argv)
       int argc;
       char **argv;
{
  void
  uperpanel
     (win,bx,panel,tb,tb_wifi,tb_eth,tb_bridge,tb_tap,wifi_menu,eth_menu,bridge_menu,tap_menu)
        Evas_Object *win;
        Evas_Object *bx;
        Evas_Object *panel;
        Evas_Object *tb;
        Evas_Object *tb_wifi;
        Evas_Object *tb_eth;
	Evas_Object *tb_bridge;
	Evas_Object *tb_tap;
        Evas_Object *wifi_menu;
        Evas_Object *eth_menu;
	Evas_Object *bridge_menu;
	Evas_Object *tap_menu;
  {
     char* wruts;
     char rbfr[4096] = "";  // large return buff for routes
     // small buff for dev type
     char wrbf[255] = "";  // wifi
     char erbf[255] = "";  // eth
     char srbf[255] = "";  // special
     char brbf[255] = "";  // bridge
     char trbf[255] = "";  // taps
     char wnme[20] = "";

     // parsse individual routes from cruts

     // upper toolbar
     tb    = elm_toolbar_add(win);

     // wireless submenu
     tb_wifi = elm_toolbar_item_append(tb, "network-wireless-acquiring", "Wireless", NULL, NULL);
     elm_toolbar_item_menu_set(tb_wifi, EINA_TRUE);
     wifi_menu  = elm_toolbar_item_menu_get(tb_wifi);

     netinf();
     w_fcz = fwifi;
     e_fcz = feth;
     b_fcz = fbrg;
     t_fcz = ftap;
     s_fcz = fspc;


     int fc = 0;
     while ( w_fcz[fc].fnme != NULL ) {

       const char* nme = w_fcz[fc].fnme;
       printf("found %s\n", nme);
       char *fce;
       sprintf(fce,"wlan%d",fc);
//       memset(wnme,0,20);
       strcat(wnme,nme);
       strcat(wnme,"\0");

       elm_menu_item_add(wifi_menu, NULL, "network-wireless-acquiring", fce, fling, fc);
       fc++;
     }

     elm_toolbar_item_priority_set(tb_wifi, 10);

     // ethernet submenu
     tb_eth = elm_toolbar_item_append(tb, "network-wired", "Ethernet", NULL, NULL);
     elm_toolbar_item_menu_set(tb_eth, EINA_TRUE);
     eth_menu  = elm_toolbar_item_menu_get(tb_eth);
     elm_menu_item_add(eth_menu, NULL, "emptytrash", "Empty Trash", NULL, NULL);
     elm_menu_item_add(eth_menu, NULL, "trashcan_full", "Full Trash", NULL, NULL);
     elm_toolbar_item_priority_set(tb_eth, 10);

     // bridge submenu
     tb_bridge = elm_toolbar_item_append(tb, "object-flip-horizontal", "Bridge", NULL, NULL);
     elm_toolbar_item_menu_set(tb_bridge, EINA_TRUE);
     bridge_menu  = elm_toolbar_item_menu_get(tb_bridge);
     elm_menu_item_add(bridge_menu, NULL, "emptytrash", "Empty Trash", NULL, NULL);
     elm_menu_item_add(bridge_menu, NULL, "trashcan_full", "Full Trash", NULL, NULL);
     elm_toolbar_item_priority_set(tb_bridge, 10);

     // tap submenu
     tb_tap = elm_toolbar_item_append(tb, "network-vpn", "Tap", NULL, NULL);
     elm_toolbar_item_menu_set(tb_tap, EINA_TRUE);
     tap_menu  = elm_toolbar_item_menu_get(tb_tap);
     elm_menu_item_add(tap_menu, NULL, "emptytrash", "Empty Trash", NULL, NULL);
     elm_menu_item_add(tap_menu, NULL, "trashcan_full", "Full Trash", NULL, NULL);
     elm_toolbar_item_priority_set(tb_tap, 10);

     elm_toolbar_item_append(tb, "application-exit", "Exit",   Exit, NULL);

     elm_toolbar_menu_parent_set(tb, win);
     evas_object_size_hint_weight_set(tb, 0.0, 0.0);
     evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, 0.0);

     elm_box_pack_end(bx, tb);
     evas_object_show(tb);

     //And move right to creating our first panel, for this panel we are going to choose the orientation as TOP and toggle it(tell it to hide itself):
     panel = elm_panel_add(win);
     elm_panel_orient_set(panel, ELM_PANEL_ORIENT_TOP);
     elm_panel_toggle(panel);
     evas_object_size_hint_weight_set(panel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
     evas_object_size_hint_align_set(panel, EVAS_HINT_FILL, EVAS_HINT_FILL);
     evas_object_show(panel);
     elm_box_pack_end(bx, panel);
  }


  void
  rightpane
     (win,bx,panel2,content)
       Evas_Object *win;
       Evas_Object *bx;
       Evas_Object *panel2;
       Evas_Object *content;
  {
     //For the second panel we choose the RIGHT orientation and explicitly set the state as hidden:

     panel2  = elm_panel_add(win);
     content = elm_label_add(win);

     elm_panel_orient_set(panel2, ELM_PANEL_ORIENT_RIGHT);
     elm_panel_hidden_set(panel2, EINA_TRUE);
     evas_object_size_hint_weight_set(panel2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
     evas_object_size_hint_align_set(panel2, EVAS_HINT_FILL, EVAS_HINT_FILL);

     elm_object_text_set(content, "content");
     evas_object_show(content);
     elm_object_content_set(panel2, content);

     evas_object_show(panel2);
     elm_box_pack_end(bx, panel2);
  }


  void
  lowerpane
     (win,bx,panel3,content2)
       Evas_Object *win;
       Evas_Object *bx;
       Evas_Object *panel3;
       Evas_Object *content2;
  {
     //For our third and last panel we won't set the orientation(which means it will use the default: LEFT):

     panel3   = elm_panel_add(win);
     content2 = elm_label_add(win);

     evas_object_size_hint_weight_set(panel3, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
     evas_object_size_hint_align_set(panel3, EVAS_HINT_FILL, EVAS_HINT_FILL);

     elm_object_text_set(content2, "content2");
     evas_object_show(content2);
     elm_object_content_set(panel3, content2);

     evas_object_show(panel3);
     elm_box_pack_end(bx, panel3);
  }


  // init main, build objects
  Evas_Object *win, *bx, *panel, *panel2, *panel3,*content, *content2,*wifi_menu, *eth_menu, *bridge_menu, *tap_menu, *tb, *tb_wifi, *tb_eth, *tb_bridge, *tb_tap;
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

  win = elm_win_util_standard_add("panel", "Panel");    // super parent
  elm_win_autodel_set(win, EINA_TRUE);

  // box for window
  bx = elm_box_add(win); 				// parent
  elm_box_horizontal_set(bx, EINA_FALSE);
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, bx);
  evas_object_show(bx);

  // load panes
  uperpanel(win,bx,panel,tb,tb_wifi,tb_eth,tb_bridge,tb_tap,wifi_menu,eth_menu,bridge_menu,tap_menu);
  rightpane(win,bx,panel2,content);
  lowerpane(win,bx,panel3,content2);

  // run
  evas_object_resize(win, 480, 540);
  evas_object_show(win);
  elm_run();

  return 0;
}

ELM_MAIN()



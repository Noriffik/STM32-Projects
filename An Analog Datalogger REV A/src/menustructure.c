/*
 * menustructure.c
 *
 *  Created on: 26-apr.-2015
 *      Author: Robbe
 */
#include "menustructure.h"

static uint32_t lastMoved;
menuItem_t menu_MAIN;
menuItem_t* menu_currentRef;
xypair_t menu_currentPosition;
uint8_t menu_enable;
uint32_t menu_menuID;

void
initialize_menuStructure(void){
	menu_menuID = 0x00;
	menu_MAIN.menuID = ++menu_menuID;
	menu_MAIN.selected = 0x01;
	menu_enable = 0x00;

	menuItem_t* menu_RTC = menustructure_addItem(&menu_MAIN, "RTC", 0x00);
	menuItem_t* menu_SD = menustructure_addItem(&menu_MAIN, "SD Card", 0x00);
	menuItem_t* menu_USB = menustructure_addItem(&menu_MAIN, "USB", 0x00);
	menuItem_t* menu_AFE = menustructure_addItem(&menu_MAIN, "AFE", 0x00);
	menuItem_t* menu_TERMINAL = menustructure_addItem(&menu_MAIN, "Terminal", 0x00);

	menustructure_addItem(menu_SD, "SD Card Info", menustructure_menuFunctionSDCardinfo);
	menustructure_addItem(menu_SD, "File Structure", menustructure_menuFunctionSDFilestructure);
	menustructure_addItem(menu_SD, "SD Card test", 0x00);
	menustructure_addItem(menu_SD, "SD Card Settings", menustructure_menuFunctionSDSdsettings);

	menustructure_addItem(menu_RTC, "RTC Info", menustructure_menuFunctionRTCRtcinfo);
	menustructure_addItem(menu_RTC, "Adjust RTC", menustructure_menuFunctionRTCAdjustrtc);
	menustructure_addItem(menu_RTC, "RTC Settings", menustructure_menuFunctionRTCRtcsettings);

	menustructure_addItem(menu_USB, "USB Info", menustructure_menuFunctionUSBInfo);
	menustructure_addItem(menu_USB, "USB Test", 0x00);
	menustructure_addItem(menu_USB, "USB Settings", 0x00);

	menustructure_addItem(menu_AFE, "AFE Info", menustructure_menuFunctionAFEInfo);
	menustructure_addItem(menu_AFE, "AFE Record", menustructure_menuFunctionAFERecord);
	menustructure_addItem(menu_AFE, "AFE Settings", menustructure_menuFunctionAFEInfo);

	menustructure_addItem(menu_TERMINAL, "Terminal Info", 0x00);
	menustructure_addItem(menu_TERMINAL, "Terminal Test", 0x00);
	menustructure_addItem(menu_TERMINAL, "Terminal Settings", 0x00);

	menu_currentRef = &menu_MAIN;
}

menuItem_t*
menustructure_addItem(menuItem_t* upperMenuItem, char* label, void(*func)(void)){
	menuItem_t* tmp = calloc(1,sizeof(menuItem_t));
	if(tmp!=0){
		tmp->upperMenuItem = upperMenuItem;
		tmp->label = label;
		tmp->fptr = func;
		tmp->menuLevel = upperMenuItem->menuLevel+1;
		tmp->menuID = ++menu_menuID;
		// attach the current new item to the structure
		menustructure_attachMenuItem(upperMenuItem, tmp);
	}
	return tmp;
}

void
menustructure_attachMenuItem(menuItem_t* sourceItem, menuItem_t* attachItem){
	// sourceitem is for example an item from lvl one.
	// check if lowermenuitem is not null
		// if it is null-> assign attachitem to it
		// it it's not null-> go down the lower item and search for the last item

	// reference the upper menu item
	attachItem->upperMenuItem = sourceItem;

	if (sourceItem->lowerMenuItem == 0x00){
		sourceItem->lowerMenuItem = attachItem;
		attachItem->selected = 0x01;
	} else {
		menuItem_t* tmp = sourceItem->lowerMenuItem;
		while(tmp->rightMenuItem!=0x00){
			tmp = tmp->rightMenuItem;
		}
		tmp->rightMenuItem = attachItem;
		attachItem->leftMenuItem = tmp;
	}
}

void
menustructure_showMenu(menuItem_t* parentItem){
	if(parentItem->lowerMenuItem != 0x00){
		// show all the lower menu items
		xypair_t startPos = {OLED_MENUWRITING_START};
		xypair_t endPos = {OLED_MENUWRITING_END};
		xypair_t curPos;
		struct FONT_DEF font = OLED_MENUWRITING_FONT;
		uint32_t minWidth = OLED_MENUWRITING_MINIMUMWIDTH_CHARSIZE*(font.u8Width+1);

		// calculate the lines possible to fill the height of the screen
		uint32_t lineCount = startPos.y / (font.u8Height);
		if( (lineCount*(font.u8Height) + (lineCount-1)) <= startPos.y){
			lineCount++;
		}

		// get total items, position of selected item, max stringlength
		menuItem_t* tmp = parentItem->lowerMenuItem;
		uint32_t totalCount = 0x00;
		uint32_t selectedPos = 0x00;
		uint32_t strLength = 0x00;
		while(tmp!=0x00){
			// search for the total itemcount
			totalCount++;

			if(tmp->selected == 0x01){
				// search for the position of the selected item in the total itemcount
				if(selectedPos!=0x00){
					tmp->selected = 0x00;
				} else {
					selectedPos = totalCount;
				}
			}

			if(totalCount<= lineCount){
				strLength = MAX(strLength, strlen(tmp->label));
			}
			tmp = tmp->rightMenuItem;
		}
			// define the actual max stringwidth
			strLength *= (font.u8Width+1);
			strLength = MAX(strLength, minWidth);

		// print every item out
			menuItem_t* startItem = parentItem->lowerMenuItem;
			curPos.x = startPos.x;
			curPos.y = startPos.y-2;
			while(startItem !=0x00){
				// take the previous position
				// check if it's possible to write the string
					if(curPos.y>=(endPos.y+font.u8Height-1)){
						// center the string
							if(startItem->selected != 0x00){
								// write the string if checked
								ssd1306_setArea(curPos.x,curPos.y+2, curPos.x+strLength, curPos.y-font.u8Height+1);
								ssd1306_setStringInvertedCentered(curPos.y-font.u8Height,
										startPos.x, endPos.x,
										startItem->label,
										font,
										0);
							} else {
								// write the string if not checked
								ssd1306_setStringCentered(curPos.y-font.u8Height, startPos.x, endPos.x, startItem->label,font);
							}
							// set the new position
							curPos.y -= (font.u8Height+2);
					}
					startItem = startItem->rightMenuItem;
			}
			menu_enable = 0x01;
	} else if (parentItem->fptr != 0x00){
		// go to function
		parentItem->fptr();
	}
}

void
menustructure_printMenuItem(menuItem_t* menuStructure){
	if(strlen(menuStructure->label)>8){
		printf("Label:%s\tLevel:%d\tSelected:%d\tID:%ld\tUP:0x%lx\tDOWN:0x%08lx\tLeft:0x%08lx\tRight:0x%08lx\tfptr:0x%08lx\r\n",
				menuStructure->label, menuStructure->menuLevel, menuStructure->selected, menuStructure->menuID, (uint32_t)menuStructure->upperMenuItem
				, (uint32_t)menuStructure->lowerMenuItem, (uint32_t)menuStructure->leftMenuItem, (uint32_t)menuStructure->rightMenuItem, (uint32_t)menuStructure->fptr);
	} else {
		printf("Label:%s\t\tLevel:%d\tSelected:%d\tID:%ld\tUP:0x%08lx\tDOWN:0x%08lx\tLeft:0x%08lx\tRight:0x%08lx\tfptr:0x%08lx\r\n",
				menuStructure->label, menuStructure->menuLevel, menuStructure->selected, menuStructure->menuID, (uint32_t)menuStructure->upperMenuItem
				, (uint32_t)menuStructure->lowerMenuItem, (uint32_t)menuStructure->leftMenuItem, (uint32_t)menuStructure->rightMenuItem, (uint32_t)menuStructure->fptr);
	}
}

void
menustructure_printMenuItemRow(menuItem_t* menuStructure){
	// recursive function
	menuItem_t* tmp = menuStructure;
	while(tmp!=0x00){
		menustructure_printMenuItem(tmp);
		menustructure_printMenuItemRow(tmp->lowerMenuItem);
		tmp = tmp->rightMenuItem;
	}
}

void
menustructure_render(void){
	if(menu_enable != 0x00 && initialization_list_STATES[initialization_list_SSD1306] != 0x00 && initialization_list_STATES[initialization_list_General] != 0x00){
		// clear the screen
		ssd1306_clearArea(OLED_MENUWRITING_START, OLED_MENUWRITING_END);

		// show the menu items
		menustructure_showMenu(menu_currentRef);

		menu_enable = 0x00;
	}
}

void
menustructure_stepMenu(uint8_t direction){
	if(delay_getMilliDifferenceSimple(lastMoved)>PB_DEBOUNCE_DELAY_MS){
		if(direction == 0x00){
			// Stepping up (pointerwise : left)
			if(menu_currentRef->lowerMenuItem != 0x00){
				// there are items under the parent structure
				menuItem_t* tmp = menu_currentRef->lowerMenuItem;
				while(tmp->selected == 0x00){
					tmp = tmp->rightMenuItem;
				}
				if(tmp->leftMenuItem != 0x00){
					tmp->selected = 0x00;
					tmp->leftMenuItem->selected = 0x01;
				}
			}
#ifdef DBG
			printf("Stepping UP! \r\n");
#endif
			menu_enable = 0x01;
		} else if (direction == 0x01){
			// Stepping down (pointerwise: right)
			if(menu_currentRef->lowerMenuItem != 0x00){
				// there are items under the parent structure
				menuItem_t* tmp = menu_currentRef->lowerMenuItem;
				while(tmp->selected == 0x00){
					tmp = tmp->rightMenuItem;
				}
				if(tmp->rightMenuItem != 0x00){
					tmp->selected = 0x00;
					tmp->rightMenuItem->selected = 0x01;
				}
			}
#ifdef DBG
			printf("Stepping DOWN! \r\n");
#endif
			menu_enable = 0x01;
		} else if (direction == 0x02){
			// Stepping back (pointerwise: up)
			menuItem_t* tmp = menu_currentRef;
			if(tmp !=0x00){
				// look for the item that is selected
				// set that item as the parent menu item
				while(tmp->leftMenuItem!=0x00){
					tmp = tmp->leftMenuItem;
				}

				while(tmp->selected==0x00){
					tmp = tmp->rightMenuItem;
				}

				if(tmp->upperMenuItem!=0x00){
					menu_currentRef = tmp->upperMenuItem;
				}

				menu_enable = 0x01;
			}

		} else if (direction == 0x03){
			// Stepping in (pointerwise: down)
			menuItem_t* tmp = menu_currentRef->lowerMenuItem;
			if(tmp !=0x00){
				// look for the item that is selected
				// set that item as the parent menu item
				while(tmp->selected==0x00){
					tmp = tmp->rightMenuItem;
				}
				menu_currentRef = tmp;

				menu_enable = 0x01;
			}
		} else {
			// maybe you are in a functionptr..
			if(menu_currentRef->fptr !=0x00){
				// yup, you are in a function pointer..
				// now act accordingly of the function
				if(direction == 0x00){
					// stepping up

				} else if(direction == 0x01){
					// stepping down

				}

			}
		}
	}
	lastMoved = delay_getMillis();
}

void
menustructure_menuFunctionSDCardinfo(void){
	xypair_t tmp;

	// header
	tmp = ssd1306_setStringCentered(OLED_TEXTBLOCK_RIGHTUP_Y-OLED_SUBMENUWRITING_HEADER_FONT.u8Height-1,
			OLED_TEXTBLOCK_LEFTDOWN_X,
			OLED_TEXTBLOCK_RIGHTUP_X,
			"SD CARD INFO",
			OLED_SUBMENUWRITING_HEADER_FONT);

	// show sd card name
		// print the subitem
		tmp.x = OLED_TEXTBLOCK_LEFTDOWN_X;
		tmp = ssd1306_setStringBelowPreviousDifferentFont(tmp,
				OLED_SUBMENUWRITING_SPACINGPIXELS_FROMHEADER,
				"CARD NAME: ",
				OLED_SUBMENUWRITING_HEADER_FONT,
				OLED_SUBMENUWRITING_SUBITEMS_FONT);
		// get sd card name

		// print the name
		tmp = ssd1306_setString(tmp.x,tmp.y,"TESTING 124", OLED_SUBMENUWRITING_SUBITEMS_FONT);

	// show sd card size
	tmp.x = OLED_TEXTBLOCK_LEFTDOWN_X;
	tmp = ssd1306_setStringBelowPreviousSameFont(tmp,
			OLED_SUBMENUWRITING_SPACINGPIXELS_FROMSUBITEMS,
			"CARD SIZE: ",
			OLED_SUBMENUWRITING_SUBITEMS_FONT);

	// show sd card available size
	tmp.x = OLED_TEXTBLOCK_LEFTDOWN_X;
	tmp = ssd1306_setStringBelowPreviousSameFont(tmp,
			OLED_SUBMENUWRITING_SPACINGPIXELS_FROMSUBITEMS,
			"AVAIL. SIZE: ",
			OLED_SUBMENUWRITING_SUBITEMS_FONT);

	// show sd card mounted
	tmp.x = OLED_TEXTBLOCK_LEFTDOWN_X;
	tmp = ssd1306_setStringBelowPreviousSameFont(tmp,
			OLED_SUBMENUWRITING_SPACINGPIXELS_FROMSUBITEMS,
			"CARD MOUNTED: ",
			OLED_SUBMENUWRITING_SUBITEMS_FONT);

	// show sd card drive number
	tmp.x = OLED_TEXTBLOCK_LEFTDOWN_X;
	tmp = ssd1306_setStringBelowPreviousSameFont(tmp,
			OLED_SUBMENUWRITING_SPACINGPIXELS_FROMSUBITEMS,
			"DRIVE NR: ",
			OLED_SUBMENUWRITING_SUBITEMS_FONT);

}

void
menustructure_menuFunctionSDFilestructure(void){
	ssd1306_setTextBlock(OLED_TEXTBLOCK_DIMENSIONS,
			"SD FILESTRUCTURE",
			Font_8x8, 0);
	// remember current

	// map the structure in RAM to how deep you are momentarily
	//
}

void
menustructure_menuFunctionSDSdsettings(void){
	ssd1306_setTextBlock(OLED_TEXTBLOCK_DIMENSIONS,
			"SD SETTINGS",
			Font_8x8, 0);
}

void
menustructure_menuFunctionRTCRtcinfo(void){
	ssd1306_setTextBlock(OLED_TEXTBLOCK_DIMENSIONS,
			"RTC INFO",
			Font_8x8, 0);
}

void
menustructure_menuFunctionRTCAdjustrtc(void){
	ssd1306_setTextBlock(OLED_TEXTBLOCK_DIMENSIONS,
			"SET RTC",
			Font_8x8, 0);
}

void
menustructure_menuFunctionRTCRtcsettings(void){

}

void
menustructure_menuFunctionUSBInfo(void){

}

void
menustructure_menuFunctionAFEInfo(void){

}

void
menustructure_menuFunctionAFERecord(void){

}

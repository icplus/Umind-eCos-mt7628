// JavaScript Document

function writeChan(regDomain, defChannel)
{
	var start=1;
	var end=1;
	switch (regDomain)
	{
		case 1:
		case 32:
			start = 1;
			end = 13;
			break;
		case 2:
			start = 10;
			end = 11;
			break;
		case 3:
			start = 10;
			end = 13;
			break;
		case 4:
			start = 14;
			end = 14;
			break;
		case 5:
		case 31:
			start = 1;
			end = 14;
			break;
		case 6:
			start = 3;
			end = 9;
			break;
		case 7:
			start = 5;
			end = 13;
			break;
		default:
			start = 1;
			end = 11;
			break;
	}
	if (defChannel==0)
		dw('<option value="0" selected>'+ glb.auto +'</option>');
	else
		dw('<option value="0">'+ glb.auto +'</option>');
	for(chan=start; chan<=end; chan++)
	{
		if (chan == defChannel)
			dw('<option value="'+chan+'" selected>'+ chan +'</option>');
		else
			dw('<option value="'+chan+'">'+ chan +'</option>');
	}
}


function WriteMenu(menu, sel)
{
	dw('<div id="menu">');
	dw('<ul>');
	for (var i=0; i<menu.length; i++)
	{
		if (i==sel)
		{
			dw('<a href="'+ menu[i].herf +'"><li class="menu_sel1">' + menu[i].name);
			dw('<ul><li class="menu_sel2">'+ menu[i].comment +'</li></ul>');
			dw('</li></a>');
		}
		else
		{
			dw('<a href="'+ menu[i].herf +'"><li class="menu_nosel1">' + menu[i].name);
			dw('<ul><li class="menu_nosel2">'+ menu[i].comment +'</li></ul>');
			dw('</li></a>');
		}
	}
	var mHtml=MainPage;
	
	dw('<a href="'+mHtml+'"><li class="menu_nosel1">' + glb.backs);
	dw('<ul><li class="menu_nosel2">'+ glb.backsi +'</li></ul>');	
	dw('</li></a>');
	
	dw('</ul>');
	dw('</div>');
}


function selSecEasy(n)
{
	var wl_Method = get_by_id("wl_Method").selectedIndex;
	var dF=document.forms[0];
	
	show_div(false, "div_key");
	if (wl_Method!=0)
	{
		show_div(true, "div_key");
	}
}
function selSec(n)
{
	var wl_Method = get_by_id("wl_Method").selectedIndex;

	show_div(false, "div_wep");
	show_div(false, "div_wpa");

	if (wl_Method == 1)
	{
		show_div(true, "div_wep");
	}
	else if (wl_Method != 0)
	{
		show_div(true, "div_wpa");
	}
}

function checkWepKey(val)
{
	str = val.value;
	if (((str.length==5 || str.length==13) && checkStr(val)) || ((str.length==10 ||str.length==26) && checkHex(val)))
	{
		return true;
	}

	return false;
}
function checkWpaPsk(val)
{
	str = val.value;
	if (str.length < 8||str.length > 64) 
	{
		PromptMsg("errorinfo", pg_wl_bs.key2);
		val.value = "";
		val.focus();
		return false;
	}
	return true;
}

function arrSort(array){
	var num = array.length/5;
	var i=0,j=0,k=0;
	for(i=0; i<num-1; i++){
		for(j=0;j<num-1-i;j++){
			if(parseInt(array[j*5+4]) < parseInt(array[(j+1)*5+4])){
				for(k=0;k<5;k++){
					var temp = array[j*5+k];
					array[j*5+k] = array[(j+1)*5+k];
					array[(j+1)*5+k] = temp;
				}
			}
		}
	}
	return array;
}
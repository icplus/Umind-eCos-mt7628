// JavaScript Document
var week=["Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"];
var month=["January","February","March","April","May","Jun","July","August","September","October","November","December"];

var gl_title_heigh = 110;
var min_bg_heigh = 500;

var MainPage = 'index.htm';
var LoginPage = 'login.htm';
var SetokPage = 'setok.htm';
var DefaultIP = '10.10.10.1';
function dw(str)
{
	document.write(str);
}

function _T(obj)
{
	if (obj!=""&& obj!=undefined)
		dw(obj);
	else
		dw("");
}

function WriteHr()
{
	dw('<div>');
		dw('<table width="100%" border="0" cellspacing="0" cellpadding="0">');
		  dw('<tr>');
			dw('<td width="120px"><hr /></td>');
			dw('<td class="title_info">'+ pg_title.info +'</td>');
			dw('<td width="500px"><hr /></td>');
		  dw('</tr>');
		dw('</table>');
	dw('</div>');
}

function get_by_id(id)
{
	with(document)
	{
		return getElementById(id);
	}
}

function get_by_name(name)
{
	with(document)
	{
		return getElementsByName(name);
	}
}

function GotoWeb(url)
{
	window.location = url;
}


function KeyPressCode(F, e)
{
	var keynum;
	if (window.event)
		keynum = e.keyCode;
	else if (e.which)
		keynum = e.which;
	if (keynum == 13)
		CheckValue(F);
}

function includeSpace(str)
{
  for (var i=0; i<str.length; i++) {
  	if ( str.charAt(i) == ' ' ) {
	  return true;
	}
  }
  return false;
}
function ajax_get(ajUrl, needRsp)
{
	$.ajax({
		type:'get',
		url: ajUrl,
		cache: false,
		data:'rd='+Math.random(),
		success:function(responseText){
			if (needRsp)
			{
				if (responseText!= "" )
				{
					//alert(responseText);
				}
			}
		}
	});
}
var Request=new QueryString();
function QueryString(){
	var name,value,i;
	var str=location.href;
	var num=str.indexOf("?");
	if (num==-1)
	{
		return "";
	}
	str=str.substr(num+1);
	var arrtmp=str.split("&");
	for(i=0;i < arrtmp.length;i++){
		num=arrtmp[i].indexOf("=");
		if(num>0){
			name=arrtmp[i].substring(0,num);
			value=arrtmp[i].substr(num+1);
			this[name]=value;
		}
	}
}

function verifyBrowser() {
	var ms = navigator.appVersion.indexOf("MSIE");
	ie4 = (ms>0) && (parseInt(navigator.appVersion.substring(ms+5, ms+6)) >= 4);
	var ns = navigator.appName.indexOf("Netscape");
	ns= (ns>=0) && (parseInt(navigator.appVersion.substring(0,1))>=4);
	if (ie4)
		return "ie4";
	else
		if(ns)
			return "ns";
		else
			return false;
}

function show_div(show,id) {
	if(show)
		document.getElementById(id).className  = "on" ;
	else	    
		document.getElementById(id).className  = "off" ;
}

function checkHex(str){
	var len = str.length;

	for (var i=0; i<len; i++) {
		if ((str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') ){
				continue;
		}else
			return false;
	}
	return true;
}

function checkStr(str){
	var len = str.length;

	for (var i=0; i<len; i++) {
		if ((str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'z') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'Z') ){
				continue;
		}else
			return false;
	}
	return true;
}

function dateStrC(sec)
{
	var d=new Date(1000*sec);
	d.setTime(d.getTime()+d.getTimezoneOffset()*60000);
	var str=d.getFullYear()+' Year '+(1+d.getMonth())+' Month '+d.getDate()+' Day ' +d.getHours() +' Hour ' +d.getMinutes()+' Min';
	return str;
}
function formatStr(num,len)
{
	var nbits = new String(num);
	var rst = "";

	a = len - (nbits.length)

	if ( a > 0)
	{
	 for (i=0;i<a;i++)
		rst += "0";
	 rst += num;
	}
	else
	 rst = num;

	return rst
}
function parseTime(t)
{
	var str=new String("");
	var tmp=parseInt(t);
	var sec=0,min=0,hr=0,day=0;
	sec=t % 60;  //sec
	min=parseInt(t/60) % 60; //min
	hr=parseInt(t/(60*60)) % 24; //hr
	day=parseInt(t/(60*60*24)); //day

	str=(day >0? day+" Day,  ":"0 Day,  ")+(hr >0? formatStr(hr,2)+":":"00:")+(min >0? formatStr(min,2)+":":"00:")+(sec >0? formatStr(sec,2)+"":"00");
	return str;
}

function MenuEntry(name, comment, herf)
{
	this.name = name;
	this.comment = comment;
	this.herf = herf;
}
var n=0;
var wl_menu = new Array();
wl_menu[n++] = new MenuEntry(pg_home.wl_bs,		pg_home.wl_bsi,		"wl_basic.htm");
//wl_menu[n++] = new MenuEntry(pg_home.wl_se,		pg_home.wl_sei,		"wl_sec.htm");
//wl_menu[n++] = new MenuEntry(pg_home.wl_as,		pg_home.wl_asi,		"wl_adv.htm");
wl_menu[n++] = new MenuEntry(pg_home.wl_wps,	pg_home.wl_wpsi,	"wl_wps.htm");
//wl_menu[n++] = new MenuEntry(pg_home.wl_sch,	pg_home.wl_schi,	"wl_sch.htm");

n=0;
var net_menu = new Array();
net_menu[n++] = new MenuEntry(pg_home.net_lan,	"",					"lan.htm");

n=0;
var mn_menu = new Array();
mn_menu[n++] = new MenuEntry(pg_home.mn_pass,	"",					"man_pass.htm");
//mn_menu[n++] = new MenuEntry(pg_home.mn_tz,		"",					"man_tz.htm");
mn_menu[n++] = new MenuEntry(pg_home.mn_set,	"",					"man_setting.htm");
mn_menu[n++] = new MenuEntry(pg_home.mn_firm,	"",					"man_upload.htm");
mn_menu[n++] = new MenuEntry(pg_home.nm_reb,	"",					"man_reboot.htm");

function HomeMainList(menu)
{
	for (i=0; i<menu.length; i++)
	{
		dw('<tr>');
		dw('<td class="main_link">');
		dw('<a href="'+ menu[i].herf +'" >'+ menu[i].name +'</a>');
		dw('</td>');
		dw('</tr>');
	}
}

function InitBgHeigh(id)
{
	if ($(window).height() < (min_bg_heigh+gl_title_heigh))
	{
		$("#"+id).height(min_bg_heigh);
	}
	else 
	{
		$("#"+id).height($(window).height() - gl_title_heigh);
	}
}
function PromptMsg(id, msg)
{
	get_by_id(id).innerHTML = msg;
}
function createXMLObject() {
    try {
        if (window.XMLHttpRequest) {
            xmlhttp = new XMLHttpRequest();
        }
        // code for IE
        else if (window.ActiveXObject) {
            xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
        }
    } catch (e) {
        xmlhttp=false
    }
    return xmlhttp;
}
function logout()
{
	try{
	  var agt=navigator.userAgent.toLowerCase();
	  if (agt.indexOf("msie") != -1) {
		// IE clear HTTP Authentication
		document.execCommand("ClearAuthenticationCache");
	  }
	  else {
		// Let's create an xmlhttp object
		var xmlhttp = createXMLObject();
		// Let's get the force page to logout for mozilla
		xmlhttp.open("GET",".force_logout_offer_login_mozilla",true,"logout","logout");
		// Let's send the request to the server
		xmlhttp.send("");
		// Let's abort the request
		xmlhttp.abort();
	  }
	  // Let's redirect the user to the main webpage
	  window.location = LoginPage;
	} catch(e) {
	// There was an error
	alert("there was an error");
	}
}

function logout2()
{
	document.cookie = "";
	try{
	  var agt=navigator.userAgent.toLowerCase();
	  if (agt.indexOf("msie") != -1) {
		// IE clear HTTP Authentication
		document.execCommand("ClearAuthenticationCache");
	  }
	  else {
		// Let's create an xmlhttp object
		var xmlhttp = createXMLObject();
		// Let's get the force page to logout for mozilla
		xmlhttp.open("GET",".force_logout_offer_login_mozilla",true,"logout","logout");
		// Let's send the request to the server
		xmlhttp.send("");
		// Let's abort the request
		xmlhttp.abort();
	  }
	  
	} catch(e) {
		
	}
}

function ajax_form(getUrl, goUrl, ajax_data)
{
	$.ajax({
		type:'get',
		url:getUrl,
		cache: false,
		data: ajax_data+'&rd='+Math.random(),
		success:function(Text){
			location.href = goUrl; 
		},
		error:function(XMLHttpRequest, textStatus, errorThrown) {
			location.href = "http://"+DefaultIP;
		}
	});
}

function ajax_form_nobreak(getUrl, goUrl, ajax_data)
{
	$.ajax({
		type:'get',
		url:getUrl,
		cache: false,
		data: ajax_data+'&rd='+Math.random(),
		success:function(Text){
			//location.href = goUrl; 
		},
		error:function(XMLHttpRequest, textStatus, errorThrown) {
			//location.href = "http://"+DefaultIP; 
		}
	});
}

function CountLeng(idu, idl)
{
	get_by_id(idl).innerHTML = idu.value.length;
}
function unmask(checked,sp,id,name,lenNum,maxLen)
{
	var tmp=get_by_id(id).value;
	//alert(tmp);
	if(checked)
		get_by_id(sp).innerHTML='<input type="text" name="'+name+'" id="'+id+'" class="input_text" maxlength="'+maxLen+'" onkeyup="CountLeng(this, \''+lenNum+'\')" value="'+tmp+'">';
	else
		get_by_id(sp).innerHTML='<input type="password" name="'+name+'" id="'+id+'" class="input_text" onkeyup="CountLeng(this, \''+lenNum+'\')" maxlength="'+maxLen+'" value="'+tmp+'">';
	
}

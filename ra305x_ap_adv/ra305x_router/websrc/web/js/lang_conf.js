// JavaScript Document

var lang=new Object();
lang.en="English"
lang.zhcn="简体中文"
lang.port="português"
lang.span="Espa?ol"
lang.fran="Fran?ais"
var lang_num=0;

var user_lang_array=new Array();
user_lang_array[lang_num++] = new lang_entry(lang.en, 	0);
user_lang_array[lang_num++] = new lang_entry(lang.fran,	2);
user_lang_array[lang_num++] = new lang_entry(lang.span,	3);
user_lang_array[lang_num++] = new lang_entry(lang.port,	4);
user_lang_array[lang_num++] = new lang_entry(lang.zhcn,	1);


function lang_entry(name, value)
{
	this.name = name;
	this.value = value;
}

function QueryString(){
	var name,value,i;
	var str=location.href;
	var num=str.indexOf("?");
	if (num==-1)
	{
		return ;
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

/*function initLang()
{
	if (isNaN(SelLang)==false)
		user_lang = SelLang;
	switch (user_lang)
	{
		case 1:
			pg_login = ZhcnLogin;
			break;
		default:
			pg_login = EnLogin;
			break;
	}
	
}*/


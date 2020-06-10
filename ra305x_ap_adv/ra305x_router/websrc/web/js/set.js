// JavaScript Document

var CA = new Array() ;
function Cfg(i,n,v)
{
	this.i=i;
    this.n=n;
    this.v=this.o=v;
	//alert(i + " "+n+" "+v);
}

function addCfg(n,i,v)
{
	CA.length++;
    CA[CA.length-1]= new Cfg(i,n,v);
}

function idxOfCfg(kk)
{
    if (kk=='undefined') { alert("Undefined"); return -1; }
    for (var i=0; i< CA.length ;i++)
    {

        if ( CA[i].n != 'undefined' && CA[i].n==kk )
            return i;
    }
    return -1;
}

function getCfg(n)
{
	var idx=idxOfCfg(n)
	if ( idx >=0)
		return CA[idx].v ;
	else
		return "";
}

function setCfg(n,v)
{
	var idx=idxOfCfg(n)
	if ( idx >=0)
	{
		CA[idx].v = v ;
	}
//	alert(n + "= "+v);
}

function cfg2Form(f)
{
    for (var i=0;i<CA.length;i++)
    {
        var e=eval('f.'+CA[i].n);
        if ( e )
		{
			if (e.name=='undefined') continue;
			if ( e.length && e[0].type=='text' )
			{
				if (e.length==4) decomIP2(e,CA[i].v);
				else if (e.length==6) decomMAC2(e,CA[i].v);
			}
			else if ( e.length && e[0].type=='radio')
			{
				for (var j=0;j<e.length;j++)
					e[j].checked=e[j].defaultChecked=(e[j].value==CA[i].v);
			}
			else if (e.type=='checkbox')
				e.checked=e.defaultChecked=Number(CA[i].v);
			else if (e.type=='select-one')
			{
				for (var j=0;j<e.options.length;j++)
					 e.options[j].selected=e.options[j].defaultSelected=(e.options[j].value==CA[i].v);
			}
			else
				e.value=getCfg(e.name);
			if (e.defaultValue!='undefined')
				e.defaultValue=e.value;
		}
    }
}

var frmExtraElm='';
function form2Cfg(f)
{

    for (var i=0;i<CA.length;i++)
    {
        var e=eval('f.'+CA[i].n);
		if ( e )
		{
			if (e.disabled) continue;
			if ( e.length && e[0].type=='text' )
			{
				if (e.length==4) CA[i].v=combinIP2(e);
				else if (e.length==6) CA[i].v=combinMAC2(e);
			}
			else if ( e.length && e[0].type=='radio')
			{
				for (var j=0;j<e.length;j++)
					if (e[j].checked) { CA[i].v=e[j].value; break; }
			}
			else
			if (e.type=='checkbox')
				setCfg(e.name, Number(e.checked) );
			else
				setCfg(e.name, e.value);
		}
    }
}

var OUTF;
function frmHead(na,to,cmd,go)
{
	OUTF="<FORM name="+na+" action="+to+" method=POST>\n"+
	"<INPUT type=hidden name=CMD value="+cmd+">\n"+
	"<INPUT type=hidden name=GO value="+go+">\n";
}

function frmEnd()
{
	OUTF+="</FORM>\n";
}

function frmAdd(n,v)
{
	set1="<input type=hidden name="+n+" value=\"";
	v=v.replace(/\"/g,"&quot;");
	var r=new RegExp(set1+".*\n","g");
	if (OUTF.search(r) >= 0)
		OUTF=OUTF.replace(r,(set1+v+"\">\n"));
	else
		OUTF += (set1+v+"\">\n");
}

function genForm(n,a,d,g)
{
	frmHead(n,a,d,g);
	var sub=0;
    for (var i=0;i<CA.length;i++)
	{
//		alert(CA[i].v + " "+ CA[i].o)
		if (CA[i].v!=CA[i].o)
		{
			frmAdd("SET"+sub,String(CA[i].i)+"="+CA[i].v);
			sub++;
		}
	}
	if (frmExtraElm.length)
		OUTF+=frmExtraElm;
	frmExtraElm=''; //reset
	frmEnd();
	return OUTF;
}

function subForm(f1,a,d,g)
{
	var msg=genForm('OUT',a,d,g);
//alert(msg);
	var newElem = document.createElement("div");
	newElem.innerHTML = msg ;
	f1.parentNode.appendChild(newElem);
	f=document.OUT;
	f.submit();
}

function addFormElm(n,v)
{
	var set1='<input type=hidden name='+n+' value="'+v+'">\n';
	frmExtraElm += set1;
}

function add_ajax_data(n, v)
{
	v=v.replace(/\"/g,"&quot;");
	set = "&"+n+"="+v;
	return set;
}

function ajax_data(f, cmd, go)
{
	var ajax_data= "CMD="+cmd+"&GO="+go;
	
	var sub=0;
	//alert(CA.length)
	for (var i=0;i<CA.length;i++)
	{
		//if (CA[i].v!=CA[i].o)
		{
			ajax_data += add_ajax_data("SET"+sub,String(CA[i].i)+"="+CA[i].v);
			sub++;
			//alert(ajax_data)
		}
	}
	//alert(ajax_data);
	return ajax_data;
}

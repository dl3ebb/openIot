var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function initWebSocket() {
	websocket = new WebSocket(gateway);
	websocket.onclose   = onClose;
	websocket.onmessage = onMessage;
}

function onClose(event) {
	setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
	manager.handleEvent(event.data);
}

function onLoad(event) {
	loadPages();
}

/**
 * Shows a page (used by menu)
 * @param {*} pageName 
 */
function showPage (pageName) {
	var z, i, elmnt;
	z = document.getElementsByTagName("*");
	for (i = 0; i < z.length; i++) {
	  elmnt = z[i];
	  page = elmnt.getAttribute("page");
	  if (page) {
		if (page == pageName) {
			elmnt.style.display = "block";
		} else {
			elmnt.style.display = "none";
		}
	  }
	}	
}

/**
 * Loads all pages in the background
 */
function loadPages() {
	var z, i, elmnt, file, xhttp, found;
	z = document.getElementsByTagName("*");
	for (i = 0; i < z.length; i++) {
		found = false;
		elmnt = z[i];
		file = elmnt.getAttribute("include-html");
		if (file) {
			xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4) {
					if (this.status == 200) {
						elmnt.innerHTML = this.responseText;
						elmnt.style.display = "none";
					}
					if (this.status == 404) {elmnt.innerHTML = "Page not found.";}
					elmnt.removeAttribute("include-html");
					executeScripts (elmnt);
					// Needs to be recursive as included parts could have includes 
					loadPages();
				}
			}
			xhttp.open("GET", file, true);
			xhttp.send();
			return;
		}
	}
	/**
	 * Loaded everything. Show home page
	 */
	showPage ('home');
	initWebSocket();
}

/**
 * Execute scripts in pages 
 */
function executeScripts(element) {
    var scripts = element.getElementsByTagName('script');
    for (var i = 0; i < scripts.length; i++) {
        var script = document.createElement('script');
        script.type = 'text/javascript';
        if (scripts[i].src) {
            script.src = scripts[i].src;
        } else {
            script.text = scripts[i].innerHTML;
        }
        document.head.appendChild(script);
    }
}

//--------------------------------------------------------------------------
// Menu
//--------------------------------------------------------------------------
function openNav() {
	document.getElementById("navigation").style.width = "250px";
	document.getElementById("main").style.marginLeft = "250px";
  }
  
function closeNav() {
	document.getElementById("navigation").style.width = "0px";
	document.getElementById("main").style.marginLeft = "0px";
}

//--------------------------------------------------------------------------
// Elementmanager receives websocket events, splits them into single events 
// and calls the handleEvent method of the element.
//--------------------------------------------------------------------------
class ElementManager {
    constructor() {
        this.elements = [];
    }

    registerElement(element) {
        this.elements.push(element);
    }

    handleEvent(event) {
		if (event.indexOf ("@@") == -1){ 
			this.handleSingleEvent(event);
		} else {
			const singleEvents = event.split ("@@");
			for (let i = 0; i < singleEvents.length; ++i) {
				this.handleSingleEvent(singleEvents[i]);
			}
		}
    }
	
	handleSingleEvent (event) {
        const parts = event.split('|');
        if (parts.length < 2) {
            return;
        }

        const id = parts[0];
        const args = parts.slice(1);

        const element = this.elements.find(el => el.getId() === id);
        if (element) {
            element.handleEvent(args.length, args);
        } 
	}

	onClick (id) {
		const elem = this.elements.find(el => el.getId() === id);
		if (elem) {
			elem.onClick();
		} 
	}

	onChange (id) {
		const elem = this.elements.find(el => el.getId() === id);
		if (elem) {
			elem.onChange();
		} 
	}
}

const manager = new ElementManager();

//--------------------------------------------------------------------------
// Abstract baseclass for all web elements.
//--------------------------------------------------------------------------
class webElement {
    constructor(id) {
        this.id = id;
    }

    getId() {
        return this.id;
    }

    setId(newId) {
        this.id = newId;
    }

    handleEvent(argc, argv) {
        throw new Error('You have to implement the method handleEvent!');
    }
}

function showError (id) {

}

function hideError (id) {
	
}

//--------------------------------------------------------------------------
// TextInput 
//--------------------------------------------------------------------------
class TextInput extends webElement {
	constructor(id, properties) {
		super(id);
		manager.registerElement(this);
		if (properties != null) {
			this.maxlen  = (properties.maxlen == null ? 20 : properties.maxlen);
			this.width = (properties.width == null ? 150 : properties.width);
			this.password = (properties.password == null ? false : properties.password);
		} else {
			this.maxlen  = 20;
			this.width = 150;
			this.password = false;
		}
		this.render();
	}

	render() {

		var src=
			"<div class=\"input-container\">" + 
				"<input type=\"" + (this.password ? "password" : "text") + "\" id=\""+this.id+"\" style=\"width:"+ this.width+"\" maxlength=\""+ this.maxlen +"\" onChange=\"manager.onChange(\'" +this.id + "\')\" />" +
             	"<span class=\"error-icon\" id=\""+this.id+"_ei\" onMouseOver=\"showError(\'" +this.id + "\')\" onMouseMout\"hideError(\'" +this.id + "\')\" >i</span>" + 
             	"<div class=\"error-popup\" id=\""+this.id+"_ep\"></div>" + 
        	"</div>";

		var elem = document.getElementById(this.id);
		elem.innerHTML = src;
		elem.id = "";
	}

	getElement () {
		return document.getElementById(this.id);
	}
	onChange() {
		if (!this.wsEventChanged) {
			websocket.send (this.id+"|V|" + document.getElementById(this.id).value);
		}
	}

	wsChanged() {
		this.wsEventChanged = true;
		document.getElementById(this.id).dispatchEvent(new Event ('change'));
		this.wsEventChanged = false;
	}

    handleEvent(argc, argv) {
		if (argv[0] == "V"){
			if (argv[1] === undefined) {
				document.getElementById(this.id).value = "";	
			} else {
				document.getElementById(this.id).value = argv[1];	
			}
			this.wsChanged();
		} else if (argv[0] == "E"){
			const input = document.getElementById(this.id);
    		const errorIcon = document.getElementById(this.id+"_ei");
    		const errorPopup = document.getElementById(this.id+"_ep");
			errorPopup.innerHTML = argv[1];
			if (argv[1] == "") {
				//--- Fehlerstatus zurücksetzen -----------------------
				input.classList.remove('error');
				errorIcon.style.display = 'none';
			} else {
				//--- Fehlerstatus setzen -----------------------
				input.classList.add('error');
				errorIcon.style.display = 'block';
			}
		}
	}
}

//--------------------------------------------------------------------------
// IntInput 
//--------------------------------------------------------------------------
class IntInput extends webElement {
	constructor(id, properties) {
		super(id);
		manager.registerElement(this);
		if (properties != null) {
			this.width = (properties.width == null ? 150 : properties.width);
			this.min = (properties.min == null ? -999999999999 : properties.min);
			this.max = (properties.max == null ? 999999999999 : properties.max);
		} else {
			this.width = 150;
			this.min =  -999999999999;
			this.max = 999999999999;
		}
		this.render();
	}

	render() {

		var src=
			"<div class=\"input-container\">" + 
			    "<input type=\"number\" id=\""+this.id+"\" ";
				if (this.min != -999999999999) {
					src += "min=\"" + this.min + "\" ";
				}
				if (this.max != -999999999999) {
					src += "max=\"" + this.max + "\" ";
				}
				src += "style=\"width:"+ this.width+"\" maxlength=\""+ this.maxlen +"\" onChange=\"manager.onChange(\'" +this.id + "\')\" />" +
             	"<span class=\"error-icon\" id=\""+this.id+"_ei\" onMouseOver=\"showError(\'" +this.id + "\')\" onMouseMout\"hideError(\'" +this.id + "\')\" >i</span>" + 
             	"<div class=\"error-popup\" id=\""+this.id+"_ep\"></div>" + 
        	"</div>";

		var elem = document.getElementById(this.id);
		elem.innerHTML = src;
		elem.id = "";
	}

	getElement () {
		return document.getElementById(this.id);
	}

	onChange() {
		if (!this.wsEventChanged) {
			websocket.send (this.id+"|V|" + document.getElementById(this.id).value);
		}
	}

	wsChanged() {
		this.wsEventChanged = true;
		document.getElementById(this.id).dispatchEvent(new Event ('change'));
		this.wsEventChanged = false;
	}

	handleEvent(argc, argv) {
		if (argv[0] == "V"){
			if (argv[1] === undefined) {
				document.getElementById(this.id).value = 0;	
			} else {
				document.getElementById(this.id).value = argv[1];	
			}
			this.wsChanged();
		} else if (argv[0] == "E"){
			const input = document.getElementById(this.id);
    		const errorIcon = document.getElementById(this.id+"_ei");
    		const errorPopup = document.getElementById(this.id+"_ep");
			errorPopup.innerHTML = argv[1];
			if (argv[1] == "") {
				//--- Fehlerstatus zurücksetzen -----------------------
				input.classList.remove('error');
				errorIcon.style.display = 'none';
			} else {
				//--- Fehlerstatus setzen -----------------------
				input.classList.add('error');
				errorIcon.style.display = 'block';
			}
		}
	}
}

//--------------------------------------------------------------------------
// LedButton (On / Off)
//--------------------------------------------------------------------------

class LedButton extends webElement {

	constructor(id, properties) {
		super(id);
		manager.registerElement(this);
		if (properties != null) {
			this.size        = (properties.size == null ? 20 : properties.size);
			this.bordercolor = (properties.bordercolor == null ? "red" : properties.bordercolor);
			this.centercolor = (properties.centercolor == null ? "black" : properties.centercolor);
		} else {
			this.size        = 20;
			this.bordercolor = "red";
			this.centercolor = "black";
		}
		this.render();
	}

    render() {
		var half = this.size / 2;
		var ledsize = half - 2;
		var src=
			"<svg height=\"" + this.size + "\" width=\"" + this.size + "\" onclick=\"manager.onClick(\'" +this.id + "\')\">" + 
				"<circle r=\"" + half + "\" cx=\"" + half + "\" cy=\"" + half + "\" fill=\"" + this.bordercolor + "\" />" +
				"<circle id=\"" + this.id + "_ctr\" r=\"" + ledsize + "\" cx=\"" + half + "\" cy=\"" + half + "\" fill=\"" + this.centercolor + "\" />" +
			"</svg>";
		var elem = document.getElementById(this.id);
		elem.innerHTML = src;
	}

	getElement () {
		return document.getElementById(this.id);
	}

	onClick() {
		if (!this.wsEventChanged) {
			var elem = document.getElementById(this.id + "_ctr");
			if (elem.style.display === "none") {
				websocket.send (this.id+"|0");
			} else {
				websocket.send (this.id+"|1");
			}
		}
	}

	wsChanged() {
		this.wsEventChanged = true;
		document.getElementById(this.id).dispatchEvent(new Event ('change'));
		this.wsEventChanged = false;
	}

	handleEvent(argc, argv) {
		var elem = document.getElementById(this.id + "_ctr");
		if (argv[0] == "V"){
			if (argv[1] == 0) {
				elem.style.display = "block";
			} else {
				elem.style.display = "none";
			}
			this.wsChanged();
		}
    }
}

//--------------------------------------------------------------------------
// Button
//--------------------------------------------------------------------------
class Button extends webElement {

	constructor(id, caption, properties) {
		super(id);
		this.caption = caption;
		if (properties != null) {
			this.confirm = (properties.confirm == null ? "" : properties.confirm);
		} else {
			this.confirm = "";
		}
		manager.registerElement(this);
		this.render();
	}

	render() {
		var src="<button id=\"" + this.id + "\">"+ this.caption +"</button>";
		var elem = document.getElementById(this.id);
		elem.innerHTML = src;
		elem.id = "";

		let lConfirm = this.confirm;
		let lId = this.id;
		if (this.confirm != "") {
			document.getElementById(this.id ).addEventListener("click", function() {
				if (confirm(lConfirm)) {
					manager.onClick (lId);
				}
			});
		} else {
			document.getElementById(this.id ).addEventListener("click", function() {
				 manager.onClick (lId);
			});
		}

	}

	getElement () {
		return document.getElementById(this.id);
	}
	
	onClick() {
		websocket.send (this.id+"|C");
	}

	handleEvent(argc, argv) {
		var elem = document.getElementById(this.id);
        var argptr = 0; 
		while (argptr < argc) {
			if (argv[argptr] == "EN"){
				//--- Enable ---------------------
				elem.disabled = false;
				argptr++;
			} else if (argv[argptr] == "DI"){
				//--- Enable ---------------------
				elem.disabled = true;
				argptr++;
			} else if (argv[argptr] == "V") {
				argptr+=2;
			} else {
				argptr++;
			}
		}
    }

}

//--------------------------------------------------------------------------
// Checkbox
//--------------------------------------------------------------------------
class CheckBox extends webElement {

	constructor(id, properties) {
		super(id);
		if (properties != null) {
			this.label = (properties.label == null ? "" : properties.label);
		} else {
			this.label = "";
		}
		manager.registerElement(this);
		this.render();
	}

	render() {
		var src="<input type=\"checkbox\" id=\""+this.id+"\" onclick=\"manager.onClick(\'" +this.id + "\')\"></input><label>" + this.label + "</label>";

		var elem = document.getElementById(this.id);
		elem.innerHTML = src;
		elem.id = "";
	}

	getElement () {
		return document.getElementById(this.id);
	}

	onClick() {
		if (!this.wsEventChanged) {
			websocket.send (this.id+"|V|" + (document.getElementById(this.id).checked ? "1" : "0"));
		}
	}

	wsChanged() {
		this.wsEventChanged = true;
		document.getElementById(this.id).dispatchEvent(new Event ('change'));
		this.wsEventChanged = false;
	}

	handleEvent(argc, argv) {
		if (argv[0] == "V"){
			if (argv[1] === undefined) {
				document.getElementById(this.id).checked = false;	
			} else {
				document.getElementById(this.id).checked = (argv[1] == 1);	
			}
			this.wsChanged();
		}
    }

}

//--------------------------------------------------------------------------
// Label
//--------------------------------------------------------------------------
class Label extends webElement {
	constructor(id, properties) {
		super(id);
		if (properties != null) {
			this.confirm = (properties.confirm == null ? "" : properties.confirm);
			this.clickable = (properties.clickable == null ? false : properties.clickable);
			if (this.confirm != null) {
				this.clickable = true;
			}
		} else {
			this.confirm = "";
			this.clickable = false;
		}
		if (this.confirm != null) {
			this.clickable = true;
		}
		manager.registerElement(this);
		this.render();
	}

	render() {
		var src="<label id=\""+this.id+"\">";
		var elem = document.getElementById(this.id);
		elem.innerHTML = src;
		elem.id = "";

		if (this.clickable != null) {
			let lConfirm = this.confirm;
			let lId = this.id;
			if (this.confirm != "") {
				document.getElementById(this.id ).addEventListener("click", function() {
            		if (confirm(lConfirm)) {
						manager.onClick (lId);
        		    }
        		});
			} else {
				document.getElementById(this.id ).addEventListener("click", function() {
					 manager.onClick (lId);
				});
			}
		}
	}

	getElement () {
		return document.getElementById(this.id);
	}

	onClick() {
		websocket.send (this.id+"|C");
	}

	handleEvent(argc, argv) {
		if (argv[0] == "V"){
			if (argv[1] === undefined) {
				document.getElementById(this.id).innerHTML = "";	
			} else {
				document.getElementById(this.id).innerHTML = argv[1];	
			}
			document.getElementById(this.id).dispatchEvent(new Event ('change'));
		}
    }

}

//--------------------------------------------------------------------------
// Selectbox
//--------------------------------------------------------------------------
class Selectbox extends webElement {
	constructor(id, properties) {
		super(id);
		if (properties != null) {
			this.width = (properties.width == null ? 150 : properties.width);
		} else {
			this.width = 150;
		}
		
		this.value = "";
		manager.registerElement(this);
		this.render();
	}

	render() {
		var src=
			"<div class=\"input-container\">" + 
				"<select id=\""+this.id+"\" style=\"width:"+ this.width+"\" onChange=\"manager.onChange(\'" +this.id + "\')\" >" +
					"<option></option>" +
				"</select>" +
				"<span class=\"error-icon\" id=\""+this.id+"_ei\" onMouseOver=\"showError(\'" +this.id + "\')\" onMouseMout\"hideError(\'" +this.id + "\')\" >i</span>" + 
				"<div class=\"error-popup\" id=\""+this.id+"_ep\"></div>" + 
        	"</div>";

		var elem = document.getElementById(this.id);
		elem.innerHTML = src;
		elem.id = "";
	}

	getElement () {
		return document.getElementById(this.id);
	}

	onChange() {
		if (!this.wsEventChanged) {
			websocket.send (this.id+"|V|"+document.getElementById(this.id).value);
		}
	}

	wsChanged() {
		this.wsEventChanged = true;
		document.getElementById(this.id).dispatchEvent(new Event ('change'));
		this.wsEventChanged = false;
	}
	
    handleEvent(argc, argv) {
		if (argv[0] == "V"){
			var sOptions = JSON.parse(argv[2]);
			var select = document.getElementById(this.id);
			if (this.value == "") {
				select.innerHTML = "<option>--- Select ---</option>";
			} else {
				select.innerHTML = "";
			}
			sOptions.forEach(function(sOpt) {
				var option = document.createElement('option');
				option.value = sOpt.value;
				option.text = sOpt.text;
				if (sOpt.value == argv[1]) {
					option.selected = "selected";
				}
				select.appendChild(option);
			  });
			  this.wsChanged();
		} else if (argv[0] == "E"){
			const input = document.getElementById(this.id);
    		const errorIcon = document.getElementById(this.id+"_ei");
    		const errorPopup = document.getElementById(this.id+"_ep");
			errorPopup.innerHTML = argv[1];
			if (argv[1] == "") {
				//--- Fehlerstatus zurücksetzen -----------------------
				input.classList.remove('error');
				errorIcon.style.display = 'none';
			} else {
				//--- Fehlerstatus setzen -----------------------
				input.classList.add('error');
				errorIcon.style.display = 'block';
			}
		} 
	}
}

/**
 * A Dialog that shows a progressbar for a number of seconds. Used during reboot
 */
class CountdownDialog extends webElement {
	constructor(id, label, seconds) {
		super(id);
		this.label = label;
		this.seconds = seconds;
		manager.registerElement(this);
		this.render();
	}
	
	render() {
		var src= 
		"<div id=\""+ this.id +"\" class=\"modal\">" + 
			"<div class=\"modal-content\">" + 
	  			"<p>" + this.label +"</p>" + 
	  			"<div id=\""+this.id+"_pc\" class=\"progress-container\">" +
					"<div id=\""+this.id+"_pb\" class=\"progress-bar\"></div>" +
		  		"</div>" + 
			"</div>" + 
  		"</div>";

		var elem = document.getElementById(this.id);
		elem.innerHTML = src;
		elem.id = "";
	}
	
	getElement () {
		return document.getElementById(this.id);
	}

	handleEvent(argc, argv) {
		if (argv[0] == "S"){
			const modal = document.getElementById(this.id);
			const modalProgressContainer = document.getElementById(this.id + "_pc");
			const modalProgressBar = document.getElementById(this.id + "_pb");
			modal.style.display = "block";
			let width = 0;
			let seconds = this.seconds;
			var interval = setInterval(function() {
				if (width >= 100) {
					clearInterval(interval);
					modal.style.display = "none";
					modal.dispatchEvent(new Event ('countdownEnd'));				
				} else {
					width += (100/seconds); 
					modalProgressBar.style.width = width + '%'; 
					modalProgressBar.innerHTML = width + '%';
				}
			}, 1000);
		}
	}
}

/**
 * A progress Dialog, that will be updated by the backend (used during the update process)
 */
class ProgressDialog extends webElement {
	constructor(id) {
		super(id);
		manager.registerElement(this);
		this.render();
	}
	
	render() {
		var src= 
		"<div id=\""+ this.id +"\" class=\"modal\">" + 
			"<div class=\"modal-content\">" + 
	  			"<p id=\""+this.id+"_lab\"></p>" + 
	  			"<div id=\""+this.id+"_pc\" class=\"progress-container\">" +
					"<div id=\""+this.id+"_pb\" class=\"progress-bar\"></div>" +
		  		"</div>" + 
			"</div>" + 
  		"</div>";

		var elem = document.getElementById(this.id);
		elem.innerHTML = src;
		elem.id = "";
	}

	getElement () {
		return document.getElementById(this.id);
	}
	
	handleEvent(argc, argv) {
		if (argv[0] == "LAB"){
	  		const modalLabel = document.getElementById(this.id + "_lab");
			modalLabel.innerHTML = argv[1];
		} else if (argv[0] == "SHOW") {
			const modal = document.getElementById(this.id);
			modal.style.display = "block";
		} else if (argv[0] == "HIDE"){
			const modal = document.getElementById(this.id);
			modal.style.display = "none";
		} else if (argv[0] == "VAL"){
			const modalProgressBar = document.getElementById(this.id + "_pb");
			modalProgressBar.style.width = argv[1] + '%'; 
			modalProgressBar.innerHTML = argv[1] + '%';
		}
	}
}

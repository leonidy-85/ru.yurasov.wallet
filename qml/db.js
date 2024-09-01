.import QtQuick.LocalStorage 2.0 as LS
Qt.include("logo.js");

//import { myFunction } from 'logo.js';


function connectDB() {
  // connect to the local database
  return LS.LocalStorage.openDatabaseSync(
    "wallet",
    "1.0",
    "Wallet Database",
    100000
  );
}

function initializeDB() {
  // initialize DB connection
  var db = connectDB();


  // run initialization queries
  db.transaction(function (tx) {
    // create table
    tx.executeSql(
      "CREATE TABLE IF NOT EXISTS barcode(id INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT, Type TEXT, Description TEXT, Code TEXT, Icon BLOB, Sel INTEGER)"
    );
    tx.executeSql("CREATE UNIQUE INDEX IF NOT EXISTS uid ON barcode(id)");

    tx.executeSql(
      "CREATE TABLE IF NOT EXISTS zint_codes(ZintCode INT, Description TEXT)"
    );
    tx.executeSql(
      "CREATE UNIQUE INDEX IF NOT EXISTS code ON zint_codes(ZintCode)"
    );
     tx.executeSql(
      "CREATE TABLE IF NOT EXISTS market(id INTEGER PRIMARY KEY AUTOINCREMENT, Zint TEXT, Name TEXT, Icon BLOB)"
    );
    tx.executeSql(
      "CREATE UNIQUE INDEX IF NOT EXISTS code ON market(id)"
    );
      var rs = tx.executeSql("SELECT * FROM zint_codes");
          if (rs.rows.length === 0) {
            initZintCodes();
          }
          var code = tx.executeSql("SELECT * FROM barcode");
          if (code.rows.length === 0) {
           //initBarcode();
          }
          var wizard = tx.executeSql("SELECT * FROM market");
          if (wizard.rows.length === 0) {
           initWizard();
          }

          if (rs.rows.length === 95) {
            // old amount of records, let's re-add
            tx.executeSql(
              "delete from zint_codes"
            );
            initZintCodes();
          }
      //    try {
      //      tx.executeSql("ALTER TABLE barcode_group ADD COLUMN IsDefault INTEGER");
      //    } catch (sqlErr) {
      //      return "Column already exists";
      //    }
        });
         mainapp.barcodeUpdate=true
        return db;
      }


function initBarcode() {
  var db = connectDB();
  db.transaction(function (tx) {
    tx.executeSql(
      'insert into barcode (Name, Type, Description, Code, Sel) values ("Code 128", "Code 128", "", "2644973319530", 1)'
    );
    tx.executeSql(
      'insert into barcode (Name, Type, Description, Code, Sel) values ("PDF417", "PDF417", "1111", "2644973319530", 1)'
    );
    tx.executeSql(
      'insert into barcode (Name, Type, Description, Code, Sel) values ("QR Code", "QR Code", "", "2644973319530", 1)'
    );
      tx.executeSql(
        'insert into barcode (Name, Type, Description, Code, Sel) values ("EAN 13", "EAN", "", "2644973319530", 1)'
      );
  });

}

/*************************************
 * Initialize the zint_codes table
 ************************************/
function initZintCodes() {
  var db = connectDB();
  db.transaction(function (tx) {
    tx.executeSql(
      'insert into zint_codes (ZintCode, Description) values (13, "EAN")'
    );
    tx.executeSql(
      'insert into zint_codes (ZintCode, Description) values (20, "Code 128")'
    )
      tx.executeSql(
      'insert into zint_codes (ZintCode, Description) values (8, "Code 39")'
    );
    tx.executeSql(
      'insert into zint_codes (ZintCode, Description) values (55, "PDF417")'
    );
    tx.executeSql(
      'insert into zint_codes (ZintCode, Description) values (58, "QR Code")'
    );
  });
}

/***************************************/
/*** SQL functions for Barcode handling ***/
/***************************************/

// select barcodes and push them into the barcodelist

function marketList(name) {
  var db = connectDB();
 var result
  db.transaction(function (tx) {
      if(name===''){
           result = tx.executeSql(
            "SELECT * FROM market ORDER BY Name COLLATE NOCASE;"
          );
      }else{
           result = tx.executeSql(
            "SELECT *  FROM market WHERE Name LIKE ?;",[name]
          );
      }

    for (var i = 0; i < result.rows.length; i++) {
        wizardPage.readMarketList(result.rows.item(i).Name, result.rows.item(i).Icon, result.rows.item(i).Zint);
    }
  });
}


//function readMarketList() {
//  var db = connectDB();

//  db.transaction(function (tx) {
//    var result = tx.executeSql(
//      "SELECT * FROM market ORDER BY Name COLLATE NOCASE;"
//    );
//    for (var i = 0; i < result.rows.length; i++) {
//        wizardPage.readMarketList(result.rows.item(i).Name, result.rows.item(i).Icon, result.rows.item(i).ZintType);
//    }
//  });
//}

// select barcodes and push them into the barcodelist
function readBarcodes(name) {
  var db = connectDB();
  db.transaction(function (tx) {
      var result
      if(name==='' || name==='%%' || name === undefined){
           result = tx.executeSql(
            "SELECT \
                 barcode.id, \
                 barcode.Name, \
                 barcode.Type, \
                 barcode.Description, \
                 barcode.Code, \
                 zint_codes.ZintCode, \
                 coalesce(barcode.Icon, '') Icon, \
                 Sel \
              FROM \
                 barcode \
                 left join \
                    zint_codes \
                    on barcode.Type = zint_codes.Description \
              ORDER BY \
                 barcode.Name COLLATE NOCASE;")
      }else{
           result = tx.executeSql(
            "SELECT \
                 barcode.id, \
                 barcode.Name, \
                 barcode.Type, \
                 barcode.Description, \
                 barcode.Code, \
                 zint_codes.ZintCode, \
                 coalesce(barcode.Icon, '') Icon, \
                 Sel \
              FROM \
                 barcode \
                 left join \
                    zint_codes \
                    on barcode.Type = zint_codes.Description \
                 WHERE Name LIKE  ? \
              ORDER BY \
                 barcode.Name COLLATE NOCASE;",[name])

      }



    for (var i = 0; i < result.rows.length; i++) {
      mainPage.appendBarcode(
        result.rows.item(i).id,
        result.rows.item(i).Name,
        result.rows.item(i).Type,
        result.rows.item(i).Description,
        result.rows.item(i).Code,
        result.rows.item(i).ZintCode,
        result.rows.item(i).Icon,
        result.rows.item(i).Sel
      );
    }
  });
}
// select barcodes for favorites
function readFavorites() {
  var db = connectDB();
    var group = 1;
  db.transaction(function (tx) {
    var result = tx.executeSql(
      "SELECT \
           barcode.id, \
           barcode.Name, \
           barcode.Type, \
           barcode.Description, \
           barcode.Code, \
           barcode.Sel, \
           zint_codes.ZintCode, \
           coalesce(barcode.Icon, '') Icon \
        FROM \
           barcode \
           left join \
              zint_codes \
              on barcode.Type = zint_codes.Description \
        WHERE Sel = 1
        ORDER BY \
           barcode.Name COLLATE NOCASE \
        LIMIT 6"
        );

    for (var i = 0; i < result.rows.length; i++) {
      mainPage.appendBarcodeFavorits(
        result.rows.item(i).id,
        result.rows.item(i).Name,
        result.rows.item(i).Type,
        result.rows.item(i).Description,
        result.rows.item(i).Code,
        result.rows.item(i).ZintCode,
        result.rows.item(i).Icon
      );
    }
  });
}


function readBarcodeList(page) {
  var db = connectDB();

  db.transaction(function (tx) {
    var result = tx.executeSql(
      "SELECT Description FROM zint_codes ORDER BY Description COLLATE NOCASE;"
    );
    for (var i = 0; i < result.rows.length; i++) {
        if(page===1){
           addCodePage.readBarcodeList(result.rows.item(i).Description, i);
        }
        else{
           editCodePage.appendBarcodeFromList(result.rows.item(i).Description, i);
        }
    }
  });
}




function removeBarcode(id) {
  var db = connectDB();

  db.transaction(function (tx) {
    var result = tx.executeSql("delete FROM barcode where id = ? ;", [id]);
    tx.executeSql("COMMIT;");
  });
     mainapp.barcodeUpdate=true
}


// insert barcode
function writeBarcode(name, type, additionally, code, icon, select) {
  var db = connectDB();
  var result;
  try {
    db.transaction(function (tx) {
      tx.executeSql(
        "INSERT INTO barcode (Name, Type, Description, Code, Sel, icon) VALUES (?, ?, ?, ?, ?, ?);",
        [name, type, additionally, code, select, icon]
      );
      tx.executeSql("COMMIT;");
    });
  mainapp.barcodeUpdate=true
   // return result.rows.item(0).Name;
  } catch (sqlErr) {
     console.error("Transaction failed: " + sqlErr.message);
    return "ERROR";
  }
}

// update barcode
function updateBarcode(
  id,
  name,
  type,
  additionally,
  code,
  icon
) {
  var db = connectDB();
  var result;
  try {
    db.transaction(function (tx) {
      tx.executeSql(
        "UPDATE barcode set Name = ?, Type = ?, Description = ?, Code = ?, Icon = ? where id = ?;",
        [name, type, additionally, code,icon, id]
      );
      tx.executeSql("COMMIT;");
      result = tx.executeSql("SELECT Name FROM barcode ");
    });
       mainapp.barcodeUpdate=true
  } catch (sqlErr) {
    return "ERROR";
  }
}


// change favorite
function changeFavorites(id,type) {
  var db = connectDB();
  db.transaction(function (tx) {
    var result = tx.executeSql("update barcode set Sel = ? where id = ?;", [
      type,
      id,
    ]);
  });
    mainapp.barcodeUpdate=true
}

// import icon as base64
function updateIcon(name, base64_image) {
  var db = connectDB();

  db.transaction(function (tx) {
    var result = tx.executeSql("update barcode set Icon = ? where Name = ?;", [
      base64_image,
      name,
    ]);
  });
}

// remove icon
function removeIcon(name) {
  var db = connectDB();

  db.transaction(function (tx) {
    var result = tx.executeSql(
      "update barcode set Icon = null where Name = ?;",
      [name]
    );
  });
}



/***************************************/
/*** Export data  JSON  ***/
/***************************************/
function db2json() {
  var db = connectDB();
  var dataList = [];
  db.transaction(
    function(tx) {
      var res = tx.executeSql("select * from barcode;");
      for (var i=0; i < res.rows.length; i++) {
        dataList.push({
          "Name": res.rows.item(i).Name,
          "Type": res.rows.item(i).Type,
          "Description": res.rows.item(i).Description,
          "Code": res.rows.item(i).Code,
          "Select": res.rows.item(i).Sel,
          "Icon": res.rows.item(i).Icon
      });
    }
  });

  if (dataList.length > 0) {
    return(JSON.stringify({"app": "wallet", "version": '3', "dataList": dataList}));
  } else {
    return("")
  }
}

/***************************************/
/*** import data JSON  ***/
/***************************************/
function json2db(jsonString, error) {
  var Icon  ;
  var json = JSON.parse(jsonString);
  error = "";
  if ((json.version !== "1" || json.version !== "2" || json.version !== "3")  && json.app !== "wallet" ) {
    error = "Unrecognized format, file is not a Wallet export";
    return(false);
  } else {
    var dataList = [];
    dataList = json.dataList;
    if (dataList.length > 0) {
      while(dataList.length > 0) {
        var Item = dataList.shift();
          if(json.version === "2" || json.version === 2){
              if(Item.Icon!==null){Icon="data:image/png;base64," + Item.Icon}
              console.log(Icon)
          }
          if(json.version === "3"){
              if(Item.Icon!==null){Icon= Item.Icon}
          }
            writeBarcode(Item.Name, Item.Type, Item.Description, Item.Code, Icon, Item.Select );
      }
     // parentPage.refreshOTPList();
      return(true);
    } else {
      error = "File empty";
      return(false);
    }
  }
}

/***************************************/
/*** notification   ***/
/***************************************/
function banner(notificationType, message) {
    notification.close()
    notification.body = message
    notification.previewSummary = notificationType
    notification.publish()
}

/***************************************/
/*** icon    ***/
/***************************************/
function img_src(barcode_icon,zint_code, st) {
    var srcImg
    var status=0

    if (barcode_icon !== "") {
        srcImg = "data:image/png;base64," + barcode_icon
    } else {
        switch (zint_code) {
        case 58:
            srcImg = st===0 ? "../icons/qrcode-icon.png" : "../icons/qrcode-icon-black.png"
            break
        case 145:
            srcImg =  st===0 ? "../icons/qrcode-icon.png" : "../icons/qrcode-icon-black.png"
            break
        case 104:
            srcImg = st===0 ? "../icons/qrcode-icon.png" : "../icons/qrcode-icon-black.png"
            break
        case 97:
            srcImg = st===0 ? "../icons/qrcode-icon.png" : "../icons/qrcode-icon-black.png"
            break
        case 71:
            srcImg = st===0 ? "../icons/datamatrix-icon.png" : "../icons/datamatrix-icon-black.png"
            break
        case 92:
            srcImg = st===0 ? "../icons/aztec-icon.png" : "../icons/aztec-icon-black.png"

            break
        default:
            srcImg = st===0 ? "../icons/barcode-icon.png" : "../icons/barcode-icon-black.png"
            break

        }
    }
    return srcImg
}
/***************************************/
/***    detect format baecode        ***/
/***************************************/
function detectFormat(code) {
    if (/^\d{13}$/.test(code)) {
        return "EAN";
    }
    else if (/^[0-9A-Fa-f]{8,}$/.test(code)) {
        return "QR Code";
    }
    else if (/^[ -~]{1,1100}$/.test(code) && code.length >= 3) {
        return "PDF417";
    }
    else if (/^[\x20-\x7E]{1,48}$/.test(code)) {
        return "Code 128";
    }
    else if (/^[A-Z0-9\-.\ \$\/\+\%]{1,43}$/.test(code)) {
           return "Code 39";
    }
    else {
        return "Unknown";
    }
}


function barcodeSize(origWidth, origHeight, maxWidth, em) {
    // find the optimal barcode size
    var width = 0;
    var height = 0;
    var scale = 0;
    // leave 0.5em border, but the barcode should be about 4em high
    while (width + em <= maxWidth && height < em * 4) {
        scale++;
        width = origWidth * scale;
        height = origHeight * scale;
    }
    if (width + em > maxWidth && scale > 1) {
        scale--;
        width = origWidth * scale;
        height = origHeight * scale;
    }
    return [width, height];
}

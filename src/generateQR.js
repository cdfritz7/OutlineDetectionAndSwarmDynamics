//qr code stuff
var http = require('http');
var fs = require('fs');
const thisqrcode = require('./qrcode.js');

const uploadURI = 'POST https://www.googleapis.com/upload/drive/v3/files?uploadType=resumable';
var metadataURI = 'POST https://www.googleapis.com/drive/v3/files'; //

//google api stuff
var fs = require('fs');
const readline = require('readline');
const {google} = require('googleapis');
var querystring = require('querystring');


// If modifying these scopes, delete token.json.
const SCOPES = ['https://www.googleapis.com/auth/drive'];
// The file token.json stores the user's access and refresh tokens, and is
// created automatically when the authorization flow completes for the first
// time.
const TOKEN_PATH = 'token.json';
var authToken;
var drive;

var uploadfilename = generateFileName(true);
fs.readFile('credentials.json', (err, content) => {
  authorize(JSON.parse(content), createFolder);
});

function generateFileName(long){
    var today = new Date();
    var y = today.getFullYear();
    // JavaScript months are 0-based.
    var m = today.getMonth() + 1;
    var d = today.getDate();
    var h = today.getHours();
    var mi = today.getMinutes();
    var s = today.getSeconds();
    if(long) return y + "_" + m + "_" + d + "_" + h + "_" + mi + "_" + s;
    else return y + "_" + m + "_" + d + "_";
}

function buildQRcode(filename){
  var goog_url_beg = "https://drive.google.com/file/d/";
  const goog_url_end = "/view?usp=sharing";

  var url = goog_url_beg + filename + goog_url_end;

  console.log(url);
  var typeNumber = 0;
  var errorCorrectionLevel = 'H';
  var qr = thisqrcode(typeNumber, errorCorrectionLevel);
  qr.addData(url);
  qr.make();
  console.log("qr code made~");
}


/**
 * Create an OAuth2 client with the given credentials, and then execute the
 * given callback function.
 * @param {Object} credentials The authorization client credentials.
 * @param {function} callback The callback to call with the authorized client.
 */
function authorize(credentials, callback) {
  const {client_secret, client_id, redirect_uris} = credentials.installed;
  const oAuth2Client = new google.auth.OAuth2(
      client_id, client_secret, redirect_uris[0]);
  // Check if we have previously stored a token.
  fs.readFile(TOKEN_PATH, (err, token) => {
    if (err) return getAccessToken(oAuth2Client, callback);
    oAuth2Client.setCredentials(JSON.parse(token));
    authToken = oAuth2Client;
    callback(oAuth2Client);
  });
}

/**
 * Get and store new token after prompting for user authorization, and then
 * execute the given callback with the authorized OAuth2 client.
 * @param {google.auth.OAuth2} oAuth2Client The OAuth2 client to get token for.
 * @param {getEventsCallback} callback The callback for the authorized client.
 */
function getAccessToken(oAuth2Client, callback) {
  const authUrl = oAuth2Client.generateAuthUrl({
    access_type: 'offline',
    scope: SCOPES,
  });
  console.log('Authorize this app by visiting this url:', authUrl);
  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });
  rl.question('Enter the code from that page here: ', (code) => {
    rl.close();
    oAuth2Client.getToken(code, (err, token) => {
      if (err) return console.error('Error retrieving access token', err);
      oAuth2Client.setCredentials(token);
      // Store the token to disk for later program executions
      fs.writeFile(TOKEN_PATH, JSON.stringify(token), (err) => {
        if (err) return console.error(err);
        console.log('Token stored to', TOKEN_PATH);
      });
      callback(oAuth2Client);
    });
  });
}

function createFolder(auth) {
  drive = google.drive({version: 'v3', auth});
  var currentDate = generateFileName(false);

  var query = "name='" + currentDate + "'";
  //search for an existing folder with today's date
  drive.files.list({
    q: query,
  }, function(err, res) {
    if(err) {
      console.error(err);
    } else {
      if(res.data.files && res.data.files.length) {
        console.log('Folder already exists for ', currentDate, ': ', res.data.files[0].id);
        uploadFile(buildQRcode);
      } else {
        var fileMetadata = {
          'name': currentDate,
          'mimeType': 'application/vnd.google-apps.folder',
          'viewersCanCopyContent': true
        };
        drive.files.create({
          resource: fileMetadata,
          fields: 'id'
        }, function (err, file) {
          if (err) {
            // Handle error
            console.error(err);
          } else {
            console.log('Folder created for ', currentDate);
            console.log('Folder', file);
          }
        });
      }
    }
  });
}

function uploadFile(callback){
  //i know this is a lot of repeat code but its far easier to do this than mess with callbacks n such
  var query = "name='" + generateFileName(false) + "'";
  var folder = drive.files.list({
    q: query,
  }, function(err, res) {
    if(err) {
      console.error(err);
    } else {
      if(res.data.files && res.data.files.length) {
        createFile(res, callback);
      } else {
        console.error("uh oh sisters!");
      }
    }
  });
}

function createFile(res, callback) {
  var foldername = res.data.files[0].id;
  var imgName = generateFileName(true);

  var fileMetadata = {
    'name': imgName,
    parents: [foldername]
  };
  var media = {
    mimeType: 'video/mp4',
    body: fs.createReadStream('test.mp4')
  };
  drive.files.create({
    resource: fileMetadata,
    media: media,
    fields: 'id'
  }, function (err, file) {
    if (err) {
      // Handle error
      console.error(err);
    } else {
      drive.permissions.create({
        fileId: file.data.id,
        resource: {
          role:"reader",
          type: "anyone",
          allowFileDiscovery: false
        }
      }, function(err,result){
        if(err) console.log(err)
      });
      callback(file.data.id); //passes the item's folder & ID for encoding in the URL
    }
  });
}

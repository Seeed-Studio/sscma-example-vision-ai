const WEBUSB_JPEG_MAGIC = 0x2b2d2b2d;
const WEBUSB_TEXT_MAGIC = 0x0f100e12;
const ALGO_OBJECT_DETECTION = 0x00;
const ALGO_OBJECT_COUNT = 0x01;
const ALGO_IMAGE_CLASSIFICATION = 0x02;
const ALGO_METER = 0x03;
const ALGO_DIGTAL_METER = 0x04;

const MODEL_PRE_INDEX_1 = 0x00;

const MODEL_PRE_INDEX_1_TARGET_0 = "Noting";
const MODEL_PRE_INDEX_1_TARGET_1 = "Panda";
const MODEL_PRE_INDEX_1_TARGET_2 = "Person";

const DEVICE_UNCONNECT = 1;
const DEVICE_CONNECTING = 2;
const DEVICE_CONNECTED = 3;

const IMG_ROTATION_NORMAL = 0;
const IMG_ROTATION_LEFT = 1;
const IMG_ROTATION_RIGHT = 2;
const IMG_ROTATION_UPSIDE_DOWN = 3;


const centerImg = new Image();
centerImg.src = './location-blue.svg';

const startImg = new Image();
startImg.src = './location-green.svg';

const endImg = new Image();
endImg.src = './location-red.svg';

let connectStatus;
let resultLabel;
let connectButton;
let configGroup;
let resultGroup;
let port;
let data_buffer = new Uint8Array(0);
let status = 0;
let recv_size = 0;
let execpt_size = 0;
let from = 0;
let to = 0.16;
let centerPointConfig,
    startPointConfig,
    endPointConfig = false;
let center = [0, 0],
    start = [0, 0],
    end = [0, 0];
// let timestamp = 0;
let precision = 2;
let result = 0;
let rotate = IMG_ROTATION_LEFT;

function computer_angler(center, start, end, from, to, point) {
    var A = Math.sqrt((start[0] - end[0]) ** 2 + (start[1] - end[1]) ** 2);
    var B = Math.sqrt((start[0] - center[0]) ** 2 + (start[1] - center[1]) ** 2);
    var C = Math.sqrt((end[0] - center[0]) ** 2 + (end[1] - center[1]) ** 2);
    var down_angle = Math.acos((B ** 2 + C ** 2 - A ** 2) / (2 * B * C));
    if (isNaN(down_angle)) {
        return 0;
    }
    var up_angle = 2 * Math.PI - down_angle;

    var a = center[1] - start[1];
    var b = start[0] - center[0];
    var c = center[0] * start[1] - start[0] * center[1];
    var d = point[0] * a + b * point[1] + c;

    var ll = Math.sqrt((point[0] - center[0]) ** 2 + (point[1] - center[1]) ** 2);
    var apoint = Math.sqrt(
        (point[0] - start[0]) ** 2 + (point[1] - start[1]) ** 2
    );

    var angle = Math.acos((B ** 2 + ll ** 2 - apoint ** 2) / (2 * B * ll));

    if (d < 0) {
        angle = 2 * Math.PI - angle;
    }

    if (isNaN(up_angle)) {
        return 0;
    }

    if (angle > up_angle) {
        var number = -1;
    } else {
        var number = (to - from) * (angle / up_angle) + from;
    }
    if (isNaN(number)) {
        return 0;
    }

    return number.toFixed(precision);
}

function concatenate(resultConstructor, ...arrays) {
    let totalLength = 0;
    for (let arr of arrays) {
        totalLength += arr.length;
    }
    let result = new resultConstructor(totalLength);
    let offset = 0;
    for (let arr of arrays) {
        result.set(arr, offset);
        offset += arr.length;
    }
    return result;
}

function Uint8ArrayToString(u8Arr) {
    var dataString = "";
    for (var i = 0; i < u8Arr.length; i++) {
        dataString += String.fromCharCode(u8Arr[i]);
    }
    return dataString;
}

function uint8arrayToBase64(u8Arr) {
    let CHUNK_SIZE = 0x8000; //arbitrary number
    let index = 0;
    let length = u8Arr.length;
    let result = "";
    let slice;
    while (index < length) {
        slice = u8Arr.subarray(index, Math.min(index + CHUNK_SIZE, length));
        result += String.fromCharCode.apply(null, slice);
        index += CHUNK_SIZE;
    }
    // web image base64: "data:image/png;base64," + b64encoded;
    // return  "data:image/png;base64," + btoa(result);
    return btoa(result);
}

function connect() {
    port.connect().then(
        () => {
            connectButton.textContent = "Disconnect";
            connectStatus = DEVICE_CONNECTED;

            document.getElementById("loading").innerHTML = "";
            port.onReceive = (data) => {
                if (data.byteLength == 8 && data.getUint32(0) == WEBUSB_JPEG_MAGIC) {
                    recv_size = 0;
                    status = 0;

                    execpt_size = parseInt(data.getUint32(4));
                    data_buffer = new Uint8Array(0);
                } else if (
                    data.byteLength == 8 &&
                    data.getUint32(0) == WEBUSB_TEXT_MAGIC
                ) {
                    recv_size = 0;
                    status = 1;

                    execpt_size = parseInt(data.getUint32(4));
                    data_buffer = new Uint8Array(0);
                } else {
                    data_buffer = concatenate(
                        Uint8Array,
                        data_buffer,
                        new Uint8Array(data.buffer)
                    );
                    recv_size += data.byteLength;
                }

                if (recv_size == execpt_size) {
                    recv_size = 0;
                    if (status == 0) {
                        var str = uint8arrayToBase64(data_buffer);
                        var canvas = document.getElementById("myCavans");
                        var outdiv = document.getElementById("outdiv");
                        var ctx = canvas.getContext("2d");
                        var img = new Image();
                        img.src = "data:image/jpeg;base64," + str;
                        img.onload = function () {
                            let width = this.naturalWidth;
                            let height = this.naturalHeight;
                            // let width = 200;
                            // let height = 200;
                            canvas.width = width;
                            canvas.height = height;
                            if (rotate == IMG_ROTATION_LEFT) {
                                ctx.translate(canvas.width * 0.5, canvas.height * 0.5);
                                ctx.rotate((-1 * Math.PI) / 2);
                                ctx.drawImage(
                                    img,
                                    -canvas.height * 0.5,
                                    -canvas.width * 0.5,
                                    canvas.width,
                                    canvas.height
                                );
                            } else if (rotate == IMG_ROTATION_RIGHT) {
                                ctx.translate(canvas.width * 0.5, canvas.height * 0.5);
                                ctx.rotate((1 * Math.PI) / 2);
                                ctx.drawImage(
                                    img,
                                    -canvas.height * 0.5,
                                    -canvas.width * 0.5,
                                    canvas.width,
                                    canvas.height
                                )
                            } else if (rotate == IMG_ROTATION_UPSIDE_DOWN) {
                                ctx.translate(canvas.width * 0.5, canvas.height * 0.5);
                                ctx.rotate((1 * Math.PI));
                                ctx.drawImage(
                                    img,
                                    -canvas.width * 0.5,
                                    -canvas.height * 0.5,
                                    canvas.width,
                                    canvas.height
                                )
                            } else {
                                ctx.drawImage(img, 0, 0, canvas.width, canvas.height);
                            }
                        };
                    } else {
                        // console.log(data_buffer)
                        var str = "" + Uint8ArrayToString(data_buffer);
                        var canvas = document.getElementById("myCavans");
                        var ctx = canvas.getContext("2d");

                        if (rotate == IMG_ROTATION_LEFT) {
                            ctx.translate(canvas.width / 2, -canvas.height / 2);
                            ctx.rotate((1 * Math.PI) / 2);
                        } else if (rotate == IMG_ROTATION_RIGHT) {
                            ctx.translate(-canvas.width / 2, canvas.height / 2);
                            ctx.rotate((-1 * Math.PI) / 2);
                        } else if (rotate == IMG_ROTATION_UPSIDE_DOWN) {
                            ctx.translate(-canvas.width / 2, -canvas.height / 2);
                            ctx.rotate((-1 * Math.PI));
                        }

                        ctx.strokeStyle = "#f00";
                        ctx.fillStyle = "#00f";
                        try {
                            configGroup.style.display = 'none';
                            resultGroup.style.display = 'none';
                            var obj = JSON.parse(str);
                            if (obj != null) {
                                console.log(str);
                                if (obj.type == "preview") {
                                    if (obj.algorithm == ALGO_OBJECT_DETECTION) {
                                        for (var i = 0; i < obj.count; i++) {
                                            ctx.font = "bold 16px arial";
                                            ctx.fillText(
                                                obj.object.target[i],
                                                obj.object.x[i],
                                                obj.object.y[i]
                                            );
                                            ctx.fillText(
                                                obj.object.confidence[i],
                                                obj.object.x[i] + 20,
                                                obj.object.y[i]
                                            );
                                            ctx.strokeRect(
                                                obj.object.x[i] - obj.object.w[i] / 2,
                                                obj.object.y[i] - obj.object.h[i] / 2,
                                                obj.object.w[i],
                                                obj.object.h[i]
                                            );
                                        }
                                    } else if (obj.algorithm == ALGO_IMAGE_CLASSIFICATION) {
                                        for (var i = 0; i < obj.count; i++) {
                                            ctx.font = "bold 16px arial";
                                            if (obj.model == MODEL_PRE_INDEX_1) {
                                                switch (obj.object.target[i]) {
                                                    case 0x00:
                                                        ctx.fillText(MODEL_PRE_INDEX_1_TARGET_0, 20, 40);
                                                        break;
                                                    case 0x01:
                                                        ctx.fillText(MODEL_PRE_INDEX_1_TARGET_1, 20, 40);
                                                        break;
                                                    case 0x02:
                                                        ctx.fillText(MODEL_PRE_INDEX_1_TARGET_2, 20, 40);
                                                        break;
                                                }
                                            } else {
                                                ctx.fillText(obj.object.target[i], 20, 40);
                                            }
                                            ctx.fillText(obj.object.confidence[i], 20, 60);
                                        }
                                    } else if (obj.algorithm == ALGO_OBJECT_COUNT) {
                                        for (var i = 0; i < obj.count; i++) {
                                            ctx.font = "bold 16px arial";
                                            ctx.fillText("" + obj.object.target[i] + ":" + obj.object.count[i], 0, 20 * (i + 1));
                                        }
                                    }else if (obj.algorithm == ALGO_DIGTAL_METER) {
                                        for (var i = 0; i < obj.count; i++) {
                                            ctx.font = "bold 16px arial";
                                            ctx.fillStyle = "#0f0";
                                            ctx.fillText(
                                                obj.object.target[i],
                                                obj.object.x[i] - obj.object.w[i] / 2,
                                                obj.object.y[i] - obj.object.h[i] / 2 - 2,
                                            );
                                            ctx.strokeRect(
                                                obj.object.x[i] - obj.object.w[i] / 2,
                                                obj.object.y[i] - obj.object.h[i] / 2,
                                                obj.object.w[i],
                                                obj.object.h[i]
                                            );
                                        }

                                        resultLabel.innerText = obj.value;
                                        resultGroup.style.display = 'block';
                                    } 
                                    
                                    if (obj.algorithm == ALGO_METER) {
                                        // var ti = Date.parse(new Date()) - timestamp;
                                        // console.log(ti + "ms");
                                        for (var i = 0; i < obj.count; i++) {
                                            ctx.font = "bold 32px arial";
                                            var gle = computer_angler(center, start, end, from, to, [
                                                obj.object.x[i],
                                                obj.object.y[i],
                                            ]);
                                            // console.log([
                                            //     obj.object.x[i] - 1,
                                            //     obj.object.y[i] - 1,
                                            // ]);
                                            ctx.fillRect(
                                                obj.object.x[i] - 1,
                                                obj.object.y[i] - 1,
                                                4,
                                                4
                                            );
                                            resultLabel.innerText = gle;
                                            // ctx.fillText(gle, 0, canvas.height);
                                        }
                                        ctx.drawImage(centerImg, center[0] - 20, center[1] - 40, 40, 40);
                                        ctx.drawImage(startImg, start[0] - 20, start[1] - 40, 40, 40);
                                        ctx.drawImage(endImg, end[0] - 20, end[1] - 40, 40, 40);
                                        // timestamp = Date.parse(new Date());
                                        configGroup.style.display = 'flex';
                                        resultGroup.style.display = 'block';
                                    } 
                                } else if (obj.type == "config") {
                                    rotate = obj.rotate;
                                }
                            }
                        } catch (error) {
                            console.log(error.message);
                        }
                    }
                }
            };
            port.onReceiveError = (error) => {
                console.error(error);
                connectStatus = DEVICE_UNCONNECT;
                connectButton.textContent = "Connect";
            };
        },
        (error) => {
            console.log(error);
            connectStatus = DEVICE_UNCONNECT;
            connectButton.textContent = "Connect";
        }
    );
}

function rectClick(e) {
    if (centerPointConfig || startPointConfig || endPointConfig) {
        let offsetX = e.offsetX;
        let offsetY = e.offsetY;
        let confirmStr = "";
        if (centerPointConfig) {
            confirmStr = "confirm center point X:" + offsetX + " Y:" + offsetY;
        } else if (startPointConfig) {
            confirmStr = "confirm start point X:" + offsetX + " Y:" + offsetY;
        } else if (endPointConfig) {
            confirmStr = "confirm end point X:" + offsetX + " Y:" + offsetY;
        }
        var r = confirm(confirmStr);
        if (r == true) {
            var xInput;
            var yInput;
            if (centerPointConfig) {
                xInput = document.getElementById("centerX");
                yInput = document.getElementById("centerY");
                center = [offsetX, offsetY];
            } else if (startPointConfig) {
                xInput = document.getElementById("startX");
                yInput = document.getElementById("startY");
                start = [offsetX, offsetY];
            } else if (endPointConfig) {
                xInput = document.getElementById("endX");
                yInput = document.getElementById("endY");
                end = [offsetX, offsetY];
            }
            xInput.value = offsetX;
            yInput.value = offsetY;
        }
    }
}

function connectOnclick() {
    if (connectStatus == DEVICE_UNCONNECT) {
        serial
            .requestPort()
            .then((selectedPort) => {
                port = selectedPort;
                connect();
            })
            .catch((error) => {
                connectButton.textContent = "Connect";
            });
        connectButton.textContent = "Connecting";
        document.getElementById("loading").innerHTML =
            '<div class="double-bounce1"></div><div class="double-bounce2"></div>';
    } else if (connectStatus == DEVICE_CONNECTED) {
        if (port) {
            port.disconnect();
            connectButton.textContent = "Connect";
            port = null;
            document.getElementById("loading").innerHTML = "";
        }
    }
}

function rotationOnclick() {
    if (rotate < IMG_ROTATION_UPSIDE_DOWN) {
        rotate++;
    } else {
        rotate = IMG_ROTATION_NORMAL;
    }
}

function pointConfigOnclick(type) {
    if (type == "center") {
        centerPointConfig = !centerPointConfig;
        startPointConfig = false;
        endPointConfig = false;
    } else if (type == "start") {
        startPointConfig = !startPointConfig;
        centerPointConfig = false;
        endPointConfig = false;
    } else if (type == "end") {
        endPointConfig = !endPointConfig;
        startPointConfig = false;
        centerPointConfig = false;
    }

    updatePointBtnStatus();
}

function updatePointBtnStatus() {
    var centerPoint = document.getElementById("centerPoint");
    var startPoint = document.getElementById("startPoint");
    var endPoint = document.getElementById("endPoint");
    centerPoint.className = `set-point ${centerPointConfig ? "pressed" : "unpressed"
        }`;
    startPoint.className = `set-point ${startPointConfig ? "pressed" : "unpressed"
        }`;
    endPoint.className = `set-point ${endPointConfig ? "pressed" : "unpressed"}`;
}

function isNumber(val) {
    var regPos = /^[0-9]+.?[0-9]*/;
    return regPos.test(val);
}
function handleInputChange(e) {
    var id = e.target.id;
    var value = e.target.value;
    if (!isNumber(value)) {
        alert("Invalid number");
        return;
    }
    value = parseFloat(value);
    if (id == "centerX") {
        center = [value, center[1]];
    } else if (id == "centerY") {
        center = [center[0], value];
    } else if (id == "startX") {
        start = [value, start[1]];
    } else if (id == "startY") {
        start = [start[0], value];
    } else if (id == "endX") {
        end = [value, end[1]];
    } else if (id == "endY") {
        end = [end[0], value];
    } else if (id == "from") {
        from = value;
    } else if (id == "to") {
        to = value;
    }
}

function handleSelectChange(e) {
    precision = e.target.value;
}

document.addEventListener("DOMContentLoaded", (event) => {
    connectButton = document.getElementById("connect");
    connectStatus = DEVICE_UNCONNECT;
    connectButton.textContent = "Connect";
    updatePointBtnStatus();

    resultLabel = document.getElementById("result");
    resultLabel.innerText = result;

    configGroup = document.getElementById("configGroup");
    resultGroup = document.getElementById("resultGroup");

    document.getElementById("from").value = from;
    document.getElementById("to").value = to;

    serial.getPorts().then((ports) => {
        if (ports.length === 0) {
            connectStatus = DEVICE_UNCONNECT;
            connectButton.textContent = "Connect";
        } else {
            connectStatus = DEVICE_CONNECTING;
            connectButton.textContent = "Connecting";
            port = ports[0];
            connect();
        }
    });
});
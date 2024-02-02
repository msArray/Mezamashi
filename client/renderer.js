const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline')

function isMatchingString(inputString) {
    // 正規表現を使用してパターンを定義
    var pattern = /^Arduino: now_alarm_\d{4}$/;

    // 入力文字列とパターンを比較
    return pattern.test(inputString);
}

function extractFourDigitNumber(inputString) {
    // 正規表現を使用してパターンを定義し、キャプチャングループを使って4桁の数字を取得
    var match = inputString.match(/^Arduino: now_alarm_(\d{4})$/);

    // もし一致する部分があれば、4桁の数字を返す。一致しない場合はnullを返す
    return match ? match[1] : null;
}

function Log(data){
    let element = document.createElement('p');
    element.innerText = data;
    document.getElementById('log').appendChild(element);
}

class App {
    constructor() {
        this.port = new SerialPort({
            path: 'COM3',
            baudRate: 9600
        });

        //console.log(port.path);

        this.parser = this.port.pipe(new ReadlineParser({ delimiter: '\r\n' }))

        this.port.on('open', function () {
            Log(`Serial OPEN ${this.port.path}`);
            setInterval(() => {
                document.getElementById('now-time').innerText = new Date();

                this.port.write(new Buffer.from(`t${new Date().getHours()}${new Date().getMinutes()}\n`), function (err, results) {
                    if (err) {
                        console.log('Err: ' + err);
                        console.log('Results: ' + results);
                    }
                });
            }, 1000);
        });

        this.parser.on('data', function (data) {
            console.log('Data: ' + data);
            if (isMatchingString(data) && document.getElementById('alarm-time') != document.activeElement) {
                let AlarmTime = `${extractFourDigitNumber(data)}`;
                console.log(AlarmTime.substr(0, 2) + ":" + AlarmTime.substr(2, 2));
                document.getElementById('alarm-time').value = AlarmTime.substr(0, 2) + ":" + AlarmTime.substr(2, 2);
            }
            Log(data);
            document.getElementById('log').scrollTop = document.getElementById('log').scrollHeight;
        });

        document.getElementById('alarm-time').addEventListener('blur', () => {
            Log(`${document.getElementById('alarm-time').value.replace(":", "")}`);
            
            this.port.write(new Buffer.from(`a${document.getElementById('alarm-time').value.replace(":", "")}\n`), function (err, results) {
                if (err) {
                    console.log('Err: ' + err);
                    console.log('Results: ' + results);
                }
            });
        })
    }
}

window.addEventListener('DOMContentLoaded', () => {
    let app = [new App()];

    document.getElementById('rcnct').addEventListener('click', () => {
        app[0] = "";
        app = [new App()];
    });
});
from flask import Flask, request, jsonify, Response, render_template, send_file
import numpy as np

import io





import threading
sem = threading.Semaphore()



app = Flask(__name__)

distance = "0"
pH = "0"
ec = "0.0"

frame1 = b'0'
frame1HasChanged = False

frame2 = b'0'
frame2HasChanged = False;

l = threading.Lock()

sensorLock = threading.Lock()
sensorData = {'distance': 0, 'ec': 0.0, 'pH': 0.0}



@app.route('/')
def hello_world():
    return "Welcome to autofarmbot"

@app.route('/cam1')
def cam1():
    return render_template('index.html', parameter='Cam1')

@app.route('/cam2')
def cam2():
    return render_template('index.html', parameter='Cam2')





@app.route('/getCam1', methods=['GET'])
def getCam1():
    return send_file(
        io.BytesIO(frame1),
        mimetype='image/bmp',
        as_attachment=False,
        download_name='uploaded_image.bmp'
    )

@app.route('/getCam2', methods=['GET'])
def getCam2():
    return send_file(
        io.BytesIO(frame2),
        mimetype='image/bmp',
        as_attachment=False,
        download_name='uploaded_image.bmp'
    )

@app.route("/postFrame1", methods=["POST", "PUT"])
def postFrame1():

    global frame1
    frame1 = request.files.get('file', '').read()
    return 'Image uploaded successfully', 200

@app.route("/postFrame2", methods=["POST", "PUT"])
def postFrame2():

    global frame2
    frame2 = request.files.get('file', '').read()
    return 'Image uploaded successfully', 200



@app.route("/postSensors", methods=["POST"])
def postDistance():
    content = request.json
    dist = content['distance']
    ph = content['pH']
    eC = content['ec']

    with sensorLock:
        sensorData['distance'] = int(dist)
        sensorData['ec'] = float(eC)
        sensorData['pH'] = float(ph)



    return jsonify({
        "status": "updated sensors!"

    }), 200


@app.route("/getSensors", methods=["GET"])
def getDistance():
    d = 0
    p = 0
    e = 0

    with sensorLock:
        d = sensorData['distance']
        e = sensorData['ec']
        p = sensorData['pH']

    return jsonify({
        "distance": d,
        "pH": p,
        "ec": e

    }), 200





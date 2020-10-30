from flask import Flask, request, render_template
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy import desc, asc
from flask_restful import Api, Resource
from xml.etree import ElementTree

import sys
import logging

app = Flask(__name__)

ENV = 'dev'

if ENV == 'dev':
    app.debug = True
    app.config['SQLALCHEMY_DATABASE_URI'] = 'postgresql://postgres:password@localhost/EMonitorDB'

else:
    debug = False
    app.config[
        'SQLALCHEMY_DATABASE_URI'] = 'postgres://hzbuwdagklnpog:3d389e4927a6df93af4704c4cef861a000b85b6dac31c7f31832b56eb7daf297@ec2-54-156-53-71.compute-1.amazonaws.com:5432/dfuqu09213kv5f'

    app.logger.addHandler(logging.StreamHandler(sys.stdout))
    app.logger.setLevel(logging.ERROR)

app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

db = SQLAlchemy(app)


class EMonitor(db.Model):
    __tablename__ = 'sensor_data'
    id = db.Column(db.Integer, primary_key=True)
    msg_id = db.Column(db.String(128))
    timestamp = db.Column(db.String(128))

    temperature = db.Column(db.Float)
    humidity = db.Column(db.Float)
    pressure = db.Column(db.Float)
    light = db.Column(db.Float)

    temperature_sd = db.Column(db.Float)
    humidity_sd = db.Column(db.Float)
    pressure_sd = db.Column(db.Float)
    light_sd = db.Column(db.Float)

    def __init__(self, msg_id, timestamp, temperature, humidity,
                 pressure, light, temperature_sd, humidity_sd, pressure_sd, light_sd):
        self.msg_id = msg_id
        self.timestamp = timestamp

        self.temperature = temperature
        self.humidity = humidity
        self.pressure = pressure
        self.light = light

        self.temperature_sd = temperature_sd
        self.humidity_sd = humidity_sd
        self.pressure_sd = pressure_sd
        self.light_sd = light_sd


def extract_data_from_xml(xml_str):
    """
    Given a XML string(CAP) this extracts the 'parameter' values
    :param xml_str: XML as a string
    :return: data as dictionary {name:value}
    """
    root = ElementTree.fromstring(xml_str)
    param = root.findall('info/parameter')
    datetime = root.find('sent').text.strip()
    identifier = root.find('identifier').text.strip()
    readings = dict()
    readings['datetime'] = datetime
    readings['identifier'] = identifier
    data = {}
    for p in param:
        name = p.find('valueName').text.strip()
        value = p.find('value').text.strip()
        data[name] = float(value)
    readings["data"] = data
    return readings


def put_to_db(xml_data):
    """
    given a data dictionary this method put the data to the database
    :param xml_data: data as dictionary {name:value}
    :return: True if database write successful, otherwise False
    """
    msg_id = (xml_data['identifier'])
    timestamp = xml_data['datetime']

    temperature = xml_data['data']['temperature']
    humidity = xml_data['data']['humidity']
    pressure = xml_data['data']['pressure']
    light = xml_data['data']['lightIntensity']

    temperature_sd = xml_data['data']['temperature_sd']
    humidity_sd = xml_data['data']['humidity_sd']
    pressure_sd = xml_data['data']['pressure_sd']
    light_sd = xml_data['data']['lightIntensity_sd']

    try:
        DB_data = EMonitor(msg_id, timestamp, temperature, humidity, pressure, light,
                           temperature_sd, humidity_sd, pressure_sd, light_sd)
        db.session.add(DB_data)
        db.session.commit()
        return True

    except:
        return False


@app.route('/data', methods=['POST'])
def post():
    xml_str = request.data
    xml_data = extract_data_from_xml(xml_str)
    print("[POST] /data : ", xml_data)
    if put_to_db(xml_data):
        return "DATABASE updated"
    else:
        return "DATABASE modification failed !"


@app.route('/')
def index():
    table = EMonitor.query.order_by(desc(EMonitor.timestamp)).limit(20).all()

    datetimes = []
    ids = []
    t = []
    h = []
    p = []
    l = []

    t_sd = []
    h_sd = []
    p_sd = []
    l_sd = []
    for row in reversed(table):
        datetimes.append(row.timestamp.split('T')[1].split('+')[0])
        ids.append(row.msg_id)

        t.append(row.temperature)
        h.append(row.humidity)
        p.append(row.pressure)
        l.append(row.light)

        t_sd.append(row.temperature_sd)
        h_sd.append(row.humidity_sd)
        p_sd.append(row.pressure_sd)
        l_sd.append(row.light_sd)

    return render_template("index.html", table=table, datetimes=datetimes, ids=ids, t=t, h=h, p=p, l=l, t_sd=t_sd,
                           h_sd=h_sd, p_sd=p_sd, l_sd=l_sd)


if __name__ == "__main__":
    app.run(port=2017)

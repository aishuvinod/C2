# admin blueprint

from flask import Blueprint, request, jsonify 
from database import db 
from models import * 


admin = Blueprint("admin", __name__)

# home route that returns below text when root url is accessed
@admin.route("/")
def hello_world():
   return "<p>Hello, World! This is a test for admin??</p>"

'''
@admin.route("/admin/hello")
def admin_hello():
    return "hello world admin "

''
class Task(db.Model):
    id =  db.Column(db.Integer, primary_key = True)
    task_id = db.Column(db.String)
    status = db.Column(db.String)
    implant_guid = db.Column(db.String)
    task_opcode = db.Column(db.Integer)
    task_args = db.Column(db.String)
''   

@admin.route("/task/create", methods = ["POST"])
def rpc_create_task():
    try:
        payload = request.json
        print(payload)

        implant_id = payload["implant_id"]
        opcode = payload["opcode"]
        args = payload["args"]
        task = make_task(
                implant_id, opcode, args
                )

        print(type(task), jsonify(task ))
        print(task.task_id)
        db.session.add(task )
        db.session.commit() 
        print(f"New task Added! {task}")

        # TODO make sure implant GUID exists! 
    except Exception as e:
        #TODO logging 
        print("Bad message received ",Exception, e)
        return jsonify([])

    return jsonify( {"status": "ok", "task_id": task.task_id })


@admin.route("/task/list")
def admin_list_tasks():
    with  db.session() as session:
        tasks = list(session.query(Task).all())    
    return jsonify(tasks)


@admin.route("/implant/list")
def admin_list_implants():
    with  db.session() as session:
        tasks = list(session.query(Implant).all())    
    return jsonify(tasks)
'''



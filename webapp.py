"""
    Web app for CSEC-472 project 1

    2/20/17
    @author: Cameron Clark
"""

import flask
import MySQLdb
import os
import time
from flask import Flask, flash, render_template
from werkzeug.utils import secure_filename

class flask_db:
    """
        Contains variables for our flask app database connection

    """

    def __init__(self):
        self.db = MySQLdb.connect(host='localhost', user='root',
                         passwd='con162ess', db='webapp')


UPLOAD_FOLDER = '/var/www/CSEC472-Project/uploads'
ALLOWED_EXTENSIONS = set(['txt', 'pdf', 'png', 'jpg', 'jpeg', 'gif'])
app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
app.config['SECRET_KEY'] ='DANK'
db = flask_db()


def allowed_file(fname):
    return '.' in fname and \
        fname.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS


def get_entries():
    """
        Retrieves all the entries in our database
    """
    cur = db.db.cursor()
    cur.execute("SELECT name, date FROM names")
    entries = cur.fetchall()
    return entries


@app.route('/', methods=['GET'])
def index():
    """
        Returns the index page

        :return index.html: our homepage
    """
    entries = get_entries()
    return render_template('index.html', names=entries) 


@app.route('/uploads/<filename>')
def uploaded_file(filename):
    """ 
        Returns a file that the user uploaded
    """
    filename = secure_filename(filename)
    return flask.send_from_directory(app.config['UPLOAD_FOLDER'], filename)


@app.route('/upload', methods=['POST'])
def upload():
    """
        Handes the user 
    """
    if 'file' not in flask.request.files:
        print 'No file part'
        return render_template('index.html')
    
    f = flask.request.files['file']
    if f.filename == '':
        print 'No selected file'
        return render_template('index.html')
    
    if f and allowed_file(f.filename):
        fname = secure_filename(f.filename)
        f.save(os.path.join(app.config['UPLOAD_FOLDER'], fname))
        print 'File uploaded successfully'
    
    entries = get_entries()
    return render_template('index.html', names=entries)


@app.route('/insert', methods=['POST'])
def insert():
    """
        Function to handle a user inputting their name

    """
    name = flask.request.form.get('name')
    if not name:
        return render_template('index.html')

    cur = db.db.cursor()
    now = time.strftime("%x")
    cur.execute("INSERT INTO names VALUES('%s', '%s')" % (name, now))
    db.db.commit()

    entries = get_entries()
    return render_template('index.html', names=entries)


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)

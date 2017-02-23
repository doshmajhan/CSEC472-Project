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

UPLOAD_FOLDER = '/var/www/hmtl/uploads'
app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
app.config['SECRET_KEY'] ='DANK'

def db_connect():
    """
        Connects to our SQL database

        :returns db: the object for our database connection
    """
    db = MySQLdb.connect(host='localhost', user='root',
                         passwd='itsvariable17', db='webapp')
    return db

def get_entries():
    """
        Retrieves all the entries in our database
    """
    cur = db.cursor()
    cur.execute("SELECT name, date FROM names")
    entries = cur.fetchall()
    # add output sanitizing here
    return entries


@app.route('/', methods=['GET'])
def index():
    """
        Returns the index page

        :return index.html: our homepage
    """
    entires = get_entries()
    return render_template('index.html', names=entries) 


@app.route('/upload', methods=['POST'])
def upload():
    """
        Handes the user 
    """
    if 'file' not in flask.request.files:
        flash('No file part')
        return render_template('index.html')
    
    f = flask.request.files['file']
    if f.filename == '':
        flash('No selected file')
        return render_template('index.html')
    
    if f:
        fname = secure_filename(f.filename)
        f.save(os.path.join(app.config['UPLOAD_FOLDER'], fname))
        flash('File uploaded successfully')
    
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

    cur = db.cursor()
    now = time.strftime("%x")
    cur.execute("INSERT INTO names VALUES('%s', '%s')" % (name, now))
    db.commit()

    entries = get_entries()
    return render_template('index.html', names=entries)


if __name__ == '__main__':
    db = db_connect()
    app.run(host='0.0.0.0', port=5000)

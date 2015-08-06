"""
TODO: rewrite this, as there is a porblem with the conn being closed early unless the connection is made within the function itself when called
by the event queue
"""

import sqlite3
import os

dblocation = os.path.dirname(os.path.realpath(__file__)) + "/database.db";
language = "english"
def getDialogue(identifier):
    conn = sqlite3.connect(dblocation) #TODO: Make this read from ini file for connection location!!!!
    result = conn.execute("SELECT english, français, nederlands, hindi, pyrate FROM dialogue WHERE identifier=?;", (identifier, ))
    row = result.fetchone()
    dialogue = "invalid dialogue identifier" #TODO: Make it so that game complains much more about this in debug mode!!!!
    if(row != None):
        english, français, nederlands, hindi, pyrate = row
        dialogue = nederlands
    conn.close()
    return dialogue

"""
import sqlite3
import os
conn = sqlite3.connect(dblocation) #TODO: Make this read from ini file for connection location!!!!
dblocation = os.path.dirname(os.path.realpath(__file__)) + "/database.db";
language = "english"
def getDialogue(identifier):
    #try:
    result = conn.execute("SELECT english, français, nederlands, hindi, pyrate FROM dialogue WHERE identifier=?;", (identifier, ))
    #except sqlite3.ProgrammingError as e:
    #	print e
    row = result.fetchone()
    dialogue = "invalid dialogue identifier" #TODO: Make it so that game complains much more about this in debug mode!!!!
    if(row != None):
        english, français, nederlands, hindi, pyrate = row
        dialogue = pyrate
    return dialogue
"""

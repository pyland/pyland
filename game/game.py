import sqlite3
import os

conn = sqlite3.connect(os.path.dirname(os.path.realpath(__file__)) + "/database.db") #TODO: Make this read from ini file for connection location!!!!
language = "english"
def getDialogue(identifier):
    result = conn.execute("SELECT english, français, nederlands, hindi, pyrate FROM dialogue WHERE identifier=?;", (identifier, ))
    row = result.fetchone()
    dialogue = "invalid dialogue identifier" #TODO: Make it so that game complains much more about this in debug mode!!!!
    if(row != None):
        english, français, nederlands, hindi, pyrate = row
        dialogue = pyrate
    return dialogue

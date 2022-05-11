import jinja2
import os

def get_template(name):
    templateLoader = jinja2.FileSystemLoader(searchpath=os.path.join(os.path.dirname(__file__),"template"))
    templateEnv = jinja2.Environment(loader=templateLoader)
    
    return templateEnv.get_template(name)

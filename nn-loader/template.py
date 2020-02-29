import jinja2

def get_template(name):
    templateLoader = jinja2.FileSystemLoader(searchpath="nn-loader/template")
    templateEnv = jinja2.Environment(loader=templateLoader)
    
    return templateEnv.get_template(name)

import cgi
import hashlib
import os

PATH = "www/html/albaud"

def get_cookie(_match): 
  # Returns the value from the matching cookie or '' if not defined.
  if 'HTTP_COOKIE' in os.environ:
    cookies = os.environ['HTTP_COOKIE']
    cookies = cookies.split('; ')
    try:
        for cookie in cookies:
            (_name, _value) = cookie.split('=')
            if (_match.lower() == _name.lower()):
                return _value
    except:
        return('')
  return('')

def hash_password(password):
    # Generate a random salt
    salt = os.urandom(16)

    # Hash the password with the salt using PBKDF2
    hashed_password = hashlib.pbkdf2_hmac('sha256', password.encode('utf-8'), salt, 100000)

    # Concatenate the salt and hashed password
    stored_password = salt + hashed_password

    # Return the stored password as a string
    return stored_password.hex()


def verify_password(password, stored_password):
    # Convert the stored password back to bytes
    stored_password_bytes = bytes.fromhex(stored_password)

    # Extract the salt from the stored password
    salt = stored_password_bytes[:16]

    # Hash the password to verify
    hashed_password = hashlib.pbkdf2_hmac('sha256', password.encode('utf-8'), salt, 100000)

    # Compare the hashed passwords
    return hashed_password == stored_password_bytes[16:]


def print_file(file):
    with open(file) as e:
        print(e.read())


def get_file(file):
    with open(file) as e:
        return (e.read())

def login(form):
    lines = get_file(PATH + "/pass.txt").split("\n")
    for i in lines:
        if i == "":
            continue
        if verify_password(form["password"].value, i.split(":")[1]):

            print("Set-Cookie: TOKEN=" + i, end="\r\n\r\n")
            print_file(PATH + "/index.html")
            exit(0)
    print("invalid credentials")

def signin(form):
    lines = get_file(PATH + "/pass.txt").split("\n")
    for i in lines:
        if i.split(':')[0] == form["username"].value:
            print("username already used")
            exit(0)
    if str(form["password"].value) != str(form["confirm-password"].value):
        print("password reapeat invalid")
        exit(0)
    t = str(form["username"].value) + ":" + hash_password(str(form["password"].value))
    with open(PATH + "/pass.txt", 'a') as e:
        e.write(t + '\n')
    print("Set-Cookie: TOKEN=" + t, end="\r\n\r\n")
    print_file(PATH + "/index.html")


def main():
    if (get_cookie("TOKEN") != ''):
        lines = get_file(PATH + "/pass.txt").split("\n")
        if (get_cookie("TOKEN") in lines):
            print_file(PATH + "/index.html")
            exit(0)
    if os.environ['REQUEST_METHOD'] == 'GET':
        if (os.environ['PATH_INFO'] == "/signin"):
            print_file(PATH + "/signin.html")
        elif (os.environ['PATH_INFO'] == "/instagram"):
            print("Location: https://www.instagram.com/", end='\r\n')
            print(end='\r\n')
        elif (os.environ['PATH_INFO'] == "/google"):
            print("Location: https://www.google.com/", end='\r\n')
            print(end='\r\n')
        else:
            print_file(PATH + "/login.html")
        exit(0)

    form = cgi.FieldStorage()
    if "confirm-password" in form:
        signin(form)
    else:
        login(form)


main()

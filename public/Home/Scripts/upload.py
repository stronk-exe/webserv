from flask import Flask, render_template, request

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('upload.html')

@app.route('/uploads/a', methods=['POST'])
def upload():
    file = request.files['file']
    if file:
        file.save(file.filename)
        return 'File uploaded successfully.'
    else:
        return 'No file selected.'

if __name__ == '__main__':
    app.run(debug=True)

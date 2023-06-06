from flask import Flask, render_template
from flask_socketio import SocketIO, emit

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret_key'

socketio = SocketIO()
socketio.init_app(app)
#socketio.init_app(app, cors_allowed_origins='*')

# 命名空间,也就是
name_space = '/echo'


@app.route('/')
def index():
    return render_template('index.html')

# 接收连接push连接请求


@app.route('/push')
def push_once():
    event_name = 'echo'
    broadcasted_data = {'data': "test message!"}
    # 设置广播数据
    socketio.emit(event_name, broadcasted_data, namespace=name_space)
    return 'done!'

# websocket连接


@socketio.on('connect', namespace=name_space)
def connected_msg():
    print('client connected.')

# websocket断开连接


@socketio.on('disconnect', namespace=name_space)
def disconnect_msg():
    print('client disconnected.')

# 接收客户端消息


@socketio.on('my_event', namespace=name_space)
def mtest_message(message):
    print(message)
    emit('my_response', {'data': message, 'count': 1})


if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000, debug=True)

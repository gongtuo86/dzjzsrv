from flask import Flask, request, jsonify, render_template
from flask_socketio import SocketIO, emit
from ctypes import *
import traceback

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)

# 客户端页面


@app.route('/')
def index():
    return render_template('index.html')

# 模拟巡检任务，返回巡检结果和进度信息


def do_inspection():
    result = []
    progress = 0
    try:
        # 调用C++动态库中的函数
        my_lib = cdll.LoadLibrary('path/to/your/lib.so')
        my_lib.do_inspection.restype = POINTER(c_char)
        for i in range(10):
            # 执行巡检任务
            task_result = my_lib.do_inspection()
            result.append(task_result.decode('utf-8'))
            # 更新进度
            progress += 10
            # 发送巡检进度信息给客户端
            print(f"Send inspection progress: {progress}")
            socketio.emit('inspection_progress', {'progress': progress})
    except Exception as e:
        # 处理客户端链接断开异常
        print(f"Client connection lost: {e}")
        traceback.print_exc()
    else:
        # 如果没有异常，说明任务正常完成，发送巡检结果信息给客户端
        print(f"Send inspection result: {result}")
        socketio.emit('inspection_result', {'result': result})

# 开始巡检任务接口


@app.route('/inspection/start', methods=['POST'])
def start_inspection():
    print(request.json)
    # 获取请求参数
    device_id = request.json.get('device_id')
    # 开始执行巡检任务
    socketio.start_background_task(target=do_inspection)
    return jsonify({'status': 'success', 'task_id': 'task_123'})


if __name__ == '__main__':
    socketio.run(app, debug=True, port=5000, host='0.0.0.0')

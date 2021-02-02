from flask import Flask
from flask_restful import Resource, Api

app = Flask(__name__)
api = Api(app)


class TestRequest(Resource):
    def get(self):
        return "GET IS OK"

    def post(self):
        return "POST IS OK"

    def put(self):
        return "PUT IS OK"

    def delete(self):
        return "DELETE IS OK"

    def patch(self):
        return "PATCH IS OK"


api.add_resource(TestRequest, "/")

if __name__ == '__main__':
    app.run(debug=True)

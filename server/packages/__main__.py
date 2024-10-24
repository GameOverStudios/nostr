import os
import logging  # Importando logging
from flask import Flask, jsonify, request, send_from_directory, abort, make_response, url_for, redirect
import jwt
from datetime import datetime, timedelta
from functools import wraps
import requests
import secrets
import pytz
import re

# Importando as funções de banco de dados e utils do pacote
from packages import db, models, utils  
from packages.cloud_storage import UPLOAD_FOLDER, upload_file_local

# Importando JWTManager
from flask_jwt_extended import JWTManager  

# Importando blueprints
from packages import routes_users, routes_repositories, routes_packages, routes_ratings, routes_invites

# Importando o decorador requires_auth do arquivo auth.py
from packages.auth import requires_auth 

# Configuração de Logging
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s - %(levelname)s - %(message)s',
                    handlers=[logging.FileHandler('app.log'),
                              logging.StreamHandler()])

# Define as configurações da aplicação
app = Flask(__name__)
app.config['SECRET_KEY'] = os.environ.get('SECRET_KEY', 'sua_chave_secreta_jwt') # Usar variável de ambiente, ou um valor padrão
app.config['DATABASE_URI'] = os.environ.get('DATABASE_URI')
app.config['CLOUD_STORAGE_ENABLED'] = os.environ.get('CLOUD_STORAGE_ENABLED', 'False').lower() == 'true'
app.config['CLOUD_STORAGE_BUCKET'] = os.environ.get('CLOUD_STORAGE_BUCKET')  # Nome do bucket (se aplicável)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER # Utiliza o upload folder definido no módulo de Cloud Storage.

# Use um fuso horário UTC para datetime
UTC = pytz.utc

# Define JWT_ALGORITHM
JWT_ALGORITHM = 'HS256' 

# Função para criar a aplicação Flask
def create_app():
    app.config["JWT_SECRET_KEY"] = app.config['SECRET_KEY']
    app.config["JWT_ACCESS_TOKEN_EXPIRES"] = timedelta(hours=1)
    app.config["JWT_REFRESH_TOKEN_EXPIRES"] = timedelta(days=30)
    app.config["JWT_ALGORITHM"] = "HS256"
    jwt = JWTManager(app)

    # Inicializa a conexão com o banco de dados
    db.init_app(app)
    with app.app_context():
        db.create_all()

    # Define blueprints
    blueprints = [
        routes_repositories.bp,  # Importando blueprint repositories
        routes_packages.bp,      # Importando blueprint packages
        routes_users.bp,         # Importando blueprint users
        routes_ratings.bp,      # Importando blueprint ratings
        routes_invites.bp       # Importando blueprint invites
    ]

    # Registra blueprints
    for blueprint in blueprints:
        app.register_blueprint(blueprint)

    # Cria tratadores de erro
    @app.errorhandler(404)
    def not_found(error):
        return make_response(jsonify({'error': 'Not found'}), 404)

    @app.errorhandler(500)
    def internal_server_error(error):
        logging.exception("Erro interno do servidor: %s", error)  # Log mais detalhado
        return make_response(jsonify({'error': 'Internal Server Error'}), 500)

    # Cria função de autenticação
    @app.teardown_appcontext
    def close_db(error):  # fecha a conexão do banco de dados
        db.close_connection(error)

    @app.before_request
    def log_request_info():
        logging.info(f"Requisição recebida: {request.method} {request.path}")

    def authenticate(username, password):
        return utils.authenticate(username, password, db.get_db())

    def requires_admin(f):
        @wraps(f)
        def decorated_function(*args, **kwargs):
           if not utils.is_admin(request.user_id, db.get_db()):
               return jsonify({'error': 'Admin privileges required'}), 403
           return f(*args, **kwargs)
        return decorated_function

    def paginate(query, args=(), per_page=20):
        return utils.paginate(query, args, per_page, request, url_for)

    # Define rotas e views
    @app.route('/health', methods=['GET'])
    def health_check():
        return jsonify({"status": "OK"}), 200

    # Iniciar a aplicação (após a configuração)
    app.run(debug=True, host='0.0.0.0') # host='0.0.0.0' torna o app acessível externamente na rede

    return app

if __name__ == '__main__':
    # Cria as tabelas do banco de dados (se não existirem)
    with app.app_context():
        db.create_all()
    app = create_app()
import os
import hashlib
from google.cloud import storage

# Configuração do Cloud Storage
BUCKET_NAME = os.environ.get('CLOUD_STORAGE_BUCKET')  # Nome do bucket do Google Cloud Storage
UPLOAD_FOLDER = 'uploads' # Diretório local temporário para upload (opcional)

os.makedirs(UPLOAD_FOLDER, exist_ok=True)

def upload_file(file, filename, bucket_name):
    """Faz o upload de um arquivo para o Cloud Storage."""
    storage_client = storage.Client()
    bucket = storage_client.bucket(bucket_name)
    blob = bucket.blob(filename)
    blob.upload_from_string(file.read(), content_type=file.content_type)  # Faz o upload do arquivo
    return f"gs://{bucket_name}/{filename}"  # Retorna o URL público

def upload_file_local(file, filename):
    """Faz o upload de um arquivo para o servidor local."""
    filepath = os.path.join(UPLOAD_FOLDER, filename)
    file.save(filepath)
    return filepath

def download_file(filename, bucket_name):
    """Baixa um arquivo do Cloud Storage."""
    storage_client = storage.Client()
    bucket = storage_client.bucket(bucket_name)
    blob = bucket.blob(filename)
    return blob.download_as_string()

def download_file_local(filename):
    """Baixa um arquivo do servidor local."""
    filepath = os.path.join(UPLOAD_FOLDER, filename)
    with open(filepath, 'rb') as f:
        return f.read()

def delete_file(filename, bucket_name):
    """Exclui um arquivo do Cloud Storage."""
    storage_client = storage.Client()
    bucket = storage_client.bucket(bucket_name)
    blob = bucket.blob(filename)
    blob.delete()

def delete_file_local(filename):
    """Exclui um arquivo do servidor local."""
    filepath = os.path.join(UPLOAD_FOLDER, filename)
    if os.path.exists(filepath):
        os.remove(filepath)

def generate_download_url(filename, bucket_name, expiration=3600):
    """Gera uma URL pública para download de um arquivo, com tempo de expiração."""
    storage_client = storage.Client()
    bucket = storage_client.bucket(bucket_name)
    blob = bucket.blob(filename)
    url = blob.generate_signed_url(expiration=expiration, method='GET')
    return url

def generate_download_url_local(filename):
    """Gera uma URL para download de um arquivo no servidor local."""
    return f"/uploads/{filename}"

def generate_filename(file, prefix=None):
    """Gera um nome de arquivo único com base no hash SHA256 do arquivo."""
    file.seek(0)
    file_hash = hashlib.sha256(file.read()).hexdigest()
    file.seek(0)
    filename = file_hash + os.path.splitext(file.filename)[1]
    if prefix:
        filename = prefix + "_" + filename
    return filename


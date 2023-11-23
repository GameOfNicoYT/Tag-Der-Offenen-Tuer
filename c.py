def transform_code(file_path):
    # Öffne die Datei zum Lesen
    with open(file_path, 'r') as file:
        content = file.read()

    # Ersetze den spezifischen Code
    transformed_content = content.replace('client.println(', '').replace(');', '')

    # Entferne Backslashes vor Anführungszeichen
    transformed_content = transformed_content.replace('\\\"', '\"')

    # Speichere das Ergebnis in derselben Datei oder in einer neuen Datei
    with open(file_path, 'w') as file:
        file.write(transformed_content)

# Pfad zur Datei
file_path = 't.txt'

# Führe die Transformation durch
transform_code(file_path)

# Apri il file in modalità lettura
with open("hausdorff_dancing-faun_15110_aniso.txt", "r") as file:
    lines = file.readlines()

# Crea una lista per memorizzare le righe formattate
formatted_lines = []

# Loop attraverso le righe del file, ignorando la prima riga
for line in lines[1:]:
    # Dividi la riga utilizzando uno o più spazi come delimitatori
    parts = line.strip().split()

    # Approssima la colonna "factor" a un solo numero dopo la virgola
    parts[1] = str(round(float(parts[1]), 1))

    # Approssima le prime 5 colonne usando la notazione scientifica
    for i in range(2, 8):  # Considera solo le colonne da 2 a 6 (incluso)
        parts[i] = format(float(parts[i]), ".2e")

    # Unisci le prime 5 colonne della riga utilizzando " & "
    formatted_line = " & ".join(parts[:5])

    # Aggiungi la sesta colonna
    formatted_line += " & " + parts[7] + " \\\\"

    # Aggiungi la riga formattata alla lista
    formatted_lines.append(formatted_line)

formatted_lines = [line.replace(" & \\", " \\") for line in formatted_lines]

# Scrivi le righe formattate in un nuovo file chiamato "table.txt"
with open("table.txt", "w") as output_file:
    output_file.write("\n".join(formatted_lines))

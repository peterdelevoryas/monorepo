let output = document.getElementById('output');
let file_selector = document.getElementById('file-selector');

function read_file(file) {
    return new Promise((resolve, reject) => {
        let reader = new FileReader();
        reader.addEventListener('load', event => {
            resolve(event.target.result);
        });
        reader.readAsText(file);
    });
}

function append(list, elem) {
    let li = document.createElement("li");
    li.textContent = elem;
    list.appendChild(li);
}

function parse_time(s) {
    let [time, ampm] = s.split(" ");
    let [hour, min] = time.split(":");
    hour = parseInt(hour);
    min = parseInt(min);
    if (ampm != 'AM' && ampm != 'PM') {
        throw `Unable to parse time from "${s}"`;
    }
    if (ampm == "PM") {
        hour += 12;
    }
    return hour * 60 + min;
}

function midpoint(x, y) {
    return x + (y - x) / 2;
}

function date2day(date) {
    let [year, month, day] = date.split("-");
    return year * 365 + month * 31 + day;
}

function compare_dates(a, b) {
    a = date2day(a);
    b = date2day(b);
    if (a < b) {
        return -1;
    }
    if (a > b) {
        return 1;
    }
    return 0;
}

function compare_records(a, b) {
    if (a.class_index < b.class_index) {
        return -1;
    }
    if (a.class_index > b.class_index) {
        return 1;
    }

    let cmp = a.last_name.localeCompare(b.last_name, 'en', { sensitivity: 'base' });
    if (cmp != 0) {
        return cmp;
    }

    cmp = a.first_name.localeCompare(b.first_name, 'en', { sensitivity: 'base' });
    if (cmp != 0) {
        return cmp;
    }

    cmp = compare_dates(a.date, b.date);
    if (cmp != 0) {
        return cmp;
    }
    return 0;
}

function download(filename, text) {
    var element = document.createElement('a');
    element.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(text));
    element.setAttribute('download', filename);
  
    element.style.display = 'none';
    document.body.appendChild(element);
  
    element.click();
  
    document.body.removeChild(element);
}

async function main(event) {
    try {
        await handle_files(event);
    } catch (error) {
        error = `Exception occurred:\n${error}`;
        for (let line of error.split("\n")) {
            let element = document.createElement('li');
            element.appendChild(document.createTextNode(line));
            output.appendChild(element);
        }
    }
}

async function handle_files(event) {
    let file_list = event.target;
    let classes = [
        ["5th grade", "9:10 AM"],
        ["4th grade", "10:00 AM"],
        ["3rd grade", "11:00 AM"],
        ["Kindergarten", "1:00 PM"],
        ["1st grade", "1:30 PM"],
        ["2nd grade", "2:30 PM"],
    ];
    for (let i = 0; i < classes.length; i += 1) {
        classes[i][1] = parse_time(classes[i][1]);
    }

    let records = [];
    for (let file of file_list.files) {
        let date = file.name.split(' ')[0];

        let text = await read_file(file);
        let lines = text.split("\n");
        let field_names = lines[0].split(",");

        for (let line of lines.slice(1)) {
            if (line == "") {
                continue;
            }
            try {
                let [first_name, last_name, email, duration, entered, exited] = line.split(",");
                let entered_min = parse_time(entered);
                let exited_min = parse_time(exited);
                let time = midpoint(entered_min, exited_min);

                let class_index = -1;
                for (let i = classes.length; i > 0; i -= 1) {
                    let class_start_time = classes[i - 1][1];
                    if (time >= class_start_time) {
                        class_index = i - 1;
                        break;
                    }
                }
                let r = {
                    first_name: first_name,
                    last_name: last_name,
                    email: email,
                    entered: entered,
                    exited: exited,
                    date: date,
                    class_index: class_index,
                };
                records.push(r)
            } catch (error) {
                throw `Error processing "${line}":\n${error}`;
            }
        }
    }
    records.sort(compare_records);

    let text = "";
    for (let r of records) {
        let class_name = 'INVALID';
        if (r.class_index != -1) {
            class_name = classes[r.class_index][0];
        }
        text += `${class_name},${r.last_name},${r.first_name},${r.date},${r.entered},${r.exited},${r.email}\n`;
    }
    download('attendance.csv', text);
}

file_selector.addEventListener('change', main);

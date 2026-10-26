import pathlib, subprocess, sys, tempfile
binary=str(pathlib.Path(sys.argv[1]).resolve())
repo=pathlib.Path(__file__).resolve().parents[1]
def run(text,cwd):
    result=subprocess.run([binary],input=text,text=True,capture_output=True,cwd=cwd,timeout=10)
    assert result.returncode==0 and not result.stderr,result.stderr
    return result.stdout
with tempfile.TemporaryDirectory() as directory:
    root=pathlib.Path(directory)
    (root/'en-de.txt').write_text((repo/'en-de.txt').read_text())
    text=run('find heat\nadd Algorithm,Step-by-step procedure;Verfahren,German\nfind Algorithm\nexport German,upper.txt\nexport german,lower.txt\nexit\n',root)
    assert 'Word found: heat' in text and 'Step-by-step procedure; Verfahren' in text,text
    assert (root/'upper.txt').read_text()==(root/'lower.txt').read_text()
    assert 'Algorithm:Step-by-step procedure;Verfahren' in (root/'upper.txt').read_text()
    text=run('import upper.txt\ndelMeaning Algorithm,Step-by-step procedure,GERMAN\nexport german,one.txt\ndelTranslation Algorithm,German\nexport german,none.txt\nexit\n',root)
    assert 'Algorithm:Verfahren' in (root/'one.txt').read_text()
    assert 'Algorithm:' not in (root/'none.txt').read_text()
    text=run('add blank,,german\nfind\nexport German\nexit\n',root)
    assert text.count('Error:')==3,text
    (root/'bad.txt').write_text('German\nvalid:meaning\ninvalid row\n')
    text=run('import bad.txt\nfind valid\nexit\n',root)
    assert 'Error:' in text and 'Word not found: valid' in text,text
    text=run('add ratio,1:2,German\nexport german,ratio.txt\nimport ratio.txt\nfind ratio\nexit\n',root)
    assert 'german : 1:2' in text,text
    run('',root)
print('dictionary import/export, normalization, spelling, deletion and EOF checks passed')

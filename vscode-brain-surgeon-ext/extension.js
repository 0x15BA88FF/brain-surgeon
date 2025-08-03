const vscode = require("vscode");
const { exec } = require("child_process");

const runLint = (document, collection) => {
    if (document.languageId !== "brainfuck") return;

    exec(`brain-surgeon lint ${document.uri.fsPath}`, (err, stdout, stderr) => {
        console.log("Brain Surgeon linting");

        const diagnostics = [];
        let lintResults = [];

        try {
            lintResults = JSON.parse(stdout);
        } catch (e) {
            vscode.window.showErrorMessage("Brain Surgeon linting failed: Invalid JSON output.");
            return;
        }

        for (const item of lintResults) {
            const range = new vscode.Range(
                new vscode.Position(item.startLine - 1, item.startColumn - 1),
                new vscode.Position(item.endLine - 1, item.endColumn)
            );

            const diagnostic = new vscode.Diagnostic(
                range,
                item.message,
                {
                    "info": vscode.DiagnosticSeverity.Information,
                    "warning": vscode.DiagnosticSeverity.Warning,
                    "error": vscode.DiagnosticSeverity.Error,
                    "hint": vscode.DiagnosticSeverity.Hint,
                }[item.level.toLowerCase()]
            );

            diagnostics.push(diagnostic);
        }

        collection.set(document.uri, diagnostics);
        console.log("Brain Surgeon linted");
    });
};

const runFormat = (document) => {
    if (document.languageId !== "brainfuck") return;

    exec(`brain-surgeon fmt ${document.uri.fsPath}`, (_err, stdout, _stderr) => {
        vscode.window.showInformationMessage(stdout);
    });
};

const activate = (context) => {
    const diagnosticCollection = vscode.languages.createDiagnosticCollection("brain-surgeon");

    vscode.workspace.onDidSaveTextDocument(doc => runLint(doc, diagnosticCollection));
    vscode.workspace.onDidSaveTextDocument(doc => runFormat(doc));

    context.subscriptions.push(diagnosticCollection);

    vscode.window.showInformationMessage("Brain Surgeon is active!");
};

const deactivate = () => { };

module.exports = {
    activate,
    deactivate
};

# GitHub source repository setup

The website itself is served by the ESP32-C3, not by GitHub Pages. The lecturer's local ESP32 web-server option also asks for a repository link, so use GitHub to publish the project source and firmware.

1. Create a public GitHub repository named `SenseSafe`.
2. Upload the complete contents of this submission package, preserving the numbered folders.
3. Make sure the `04_ESP32_WebHost` folder includes the `.ino`, its `data` folder, and README.
4. Copy the repository URL into the final submission form / report if requested.
5. Do not enable GitHub Pages unless the lecturer later asks for a GitHub-hosted mirror.

Suggested Git commands, run from the extracted package folder:

```bash
git init
git add .
git commit -m "Initial SenseSafe project submission"
git branch -M main
git remote add origin https://github.com/YOUR-USERNAME/SenseSafe.git
git push -u origin main
```

Replace `YOUR-USERNAME` with the correct GitHub username. Never commit Wi-Fi passwords or private credentials. The included AP demo password is a project demonstration password and should be changed before any real deployment.

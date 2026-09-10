# CppCopier Documentation

> **Security Notice**: MetaRPC Trade Copier transmits all account credentials exclusively via secure gRPC request payloads over HTTP/2 TLS (copy.mrpc.pro:443). No trading passwords are ever stored or exposed in query parameters, server logs, or URL strings.

Official documentation for CppCopier high-performance trade replication engine.

---

## 🚀 Quick Start & Guides

- 🚀 [**Quick Start Guide**](All_Guides/Your_First_Project.md) — Your first trade copier project from scratch in 10 minutes
- ⚡ [**Getting Started**](All_Guides/GETTING_STARTED.md) — Setup, prerequisites, and gRPC connection overview
- 🗺️ [**Project Map**](All_Guides/PROJECT_MAP.md) — Architecture and component layers
- 📖 [**Glossary**](All_Guides/GLOSSARY.md) — Trade Copier terminology and concepts

---

## 🛠️ API Reference & Documentation

- 📦 [**CopierService**](API_Reference/CopierService.md) — High-level wrapper methods for Start, List, Pause, and Remove
- 🔌 [**CopierAccount**](API_Reference/CopierAccount.md) — Low-level gRPC protocol client & HTTP/2 TLS streaming
- 🪄 [**CopierSugar**](API_Reference/CopierSugar.md) — Fluent builder for one-liner trade replication
- ⚙️ [**Copier Parameters Reference**](API_Reference/Copier_Parameters.md) — Comprehensive explanation of all copier flags and settings
- 🧬 [**Input & Output Structs**](API_Reference/Input_Output_Structs.md) — Complete gRPC request and response schemas
- 🧪 [**Demo Account Creation**](API_Reference/Demo_Account_Creation.md) — Automated demo account provisioning via gRPC

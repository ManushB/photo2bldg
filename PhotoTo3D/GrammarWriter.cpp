#include "GrammarWriter.h"
#include <QFile>
#include <QDomDocument>
#include <QDomNode>
#include <QTextStream>

namespace cga {

	void save(const char* filename, const Grammar& grammar) {
		QFile file(filename);
		file.open(QFile::WriteOnly);

		QDomDocument doc;
		QDomElement root = doc.createElement("grammar");
		root.setAttribute("version", "1.0");
		root.setAttribute("author", "Gen Nishida");
		doc.appendChild(root);

		save(doc, root, grammar);
		
		QTextStream out(&file);
		QDomNode xmlNode = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"ISO-8859-1\"");
		doc.insertBefore(xmlNode, doc.firstChild());
		doc.save(out, 4);
	}

	void save(const char* filename, const std::vector<Grammar>& grammars) {
		QFile file(filename);
		file.open(QFile::WriteOnly);

		QDomDocument doc;
		QDomElement root = doc.createElement("grammars");
		root.setAttribute("version", "1.0");
		root.setAttribute("author", "Gen Nishida");
		doc.appendChild(root);

		for (int i = 0; i < grammars.size(); ++i) {
			QDomElement grammar_node = doc.createElement("grammar");
			root.appendChild(grammar_node);

			save(doc, grammar_node, grammars[i]);
		}

		QTextStream out(&file);
		QDomNode xmlNode = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"ISO-8859-1\"");
		doc.insertBefore(xmlNode, doc.firstChild());
		doc.save(out, 4);
	}

	void save(QDomDocument& doc, QDomElement& root, const Grammar& grammar) {
		// write attributes
		for (auto it = grammar.attrs.begin(); it != grammar.attrs.end(); ++it) {
			QDomElement attr = doc.createElement("attr");
			attr.setAttribute("name", it->first.c_str());
			attr.setAttribute("value", it->second.value.c_str());
			if (it->second.hasRange) {
				attr.setAttribute("range", (std::to_string(it->second.range_start) + "," + std::to_string(it->second.range_end)).c_str());
			}
			root.appendChild(attr);
		}

		// write rules
		for (auto it = grammar.rules.begin(); it != grammar.rules.end(); ++it) {
			QDomElement rule = doc.createElement("rule");
			rule.setAttribute("name", it->first.c_str());
			root.appendChild(rule);

			// write operators
			for (int k = 0; k < it->second.operators.size(); ++k) {
				QDomElement oper = it->second.operators[k]->toXml(doc);
				rule.appendChild(oper);
			}
		}
	}

}